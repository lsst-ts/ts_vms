/*
 * This file is part of LSST MT VMS package.
 *
 * Developed for the Vera C. Rubin Telescope and Site System.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <chrono>
#include <spdlog/spdlog.h>
#include <thread>
#include <unistd.h>

#ifdef SIMULATOR
#include <math.h>
#endif

#include <Events/FPGAState.h>
#include <FPGA.h>
#include <NiFpga_VMS_3_Controller.h>
#include <NiFpga_VMS_3_Responder.h>
#include <NiFpga_VMS_6_Controller.h>
#include <NiFpga_VMS_6_Responder.h>
#include <NiFpga_VMS_CameraRotator_Controller.h>
#include <NiFpga_VMS_CameraRotator_Responder.h>
#include <VMSApplicationSettings.h>
#include <VMSPublisher.h>
#include <cRIO/NiError.h>

using namespace LSST::VMS;

FPGA::FPGA(token) : SimpleFPGA(LSST::cRIO::VMS) {}

#define NiFpga_VMS_6_Responder_ControlBool_Operate -1
#define NiFpga_VMS_3_Responder_ControlBool_Operate -1

#define POPULATE_FPGA(type)                                                                    \
    _bitFile = "/var/lib/MTVMS/" NiFpga_VMS_##type##_Bitfile;                                  \
    _signature = NiFpga_VMS_##type##_Signature;                                                \
    _averageFIFO = NiFpga_VMS_##type##_TargetToHostFifoSgl_Average;                            \
    _minFIFO = NiFpga_VMS_##type##_TargetToHostFifoFxp_Min_Resource;                           \
    _maxFIFO = NiFpga_VMS_##type##_TargetToHostFifoFxp_Max_Resource;                           \
    _rawOutputFIFO = NiFpga_VMS_##type##_TargetToHostFifoFxp_RawOutput_Resource;               \
    _operateResource = NiFpga_VMS_##type##_ControlBool_Operate;                                \
    _periodResource = NiFpga_VMS_##type##_ControlU32_Periodms;                                 \
    _readyResource = NiFpga_VMS_##type##_IndicatorBool_Ready;                                  \
    _stoppedResource = NiFpga_VMS_##type##_IndicatorBool_Stopped;                              \
    _timeoutedResource = NiFpga_VMS_##type##_IndicatorBool_Timeouted;                          \
    _fifoFullResource = NiFpga_VMS_##type##_IndicatorBool_FIFOfull;                            \
    _chasisTemperatureResource = NiFpga_VMS_##type##_IndicatorFxp_ChassisTemperature_Resource; \
    _chasisTemperatureTypeInfo = NiFpga_VMS_##type##_IndicatorFxp_ChassisTemperature_TypeInfo; \
    _ticksResource = NiFpga_VMS_##type##_IndicatorU64_Ticks;

void FPGA::populate(VMSApplicationSettings *vmsApplicationSettings) {
    SPDLOG_TRACE("FPGA::FPGA()");
    _vmsApplicationSettings = vmsApplicationSettings;
    session = 0;
    remaining = 0;
    if ((_vmsApplicationSettings->Subsystem == "M1M3" ||
         _vmsApplicationSettings->Subsystem == "CameraRotator")) {
        _channels = 3;
        if (_vmsApplicationSettings->IsController) {
            POPULATE_FPGA(3_Controller);
        } else {
            POPULATE_FPGA(3_Responder);
        }
    } else if (_vmsApplicationSettings->Subsystem == "M2") {
        _channels = 6;
        if (_vmsApplicationSettings->IsController) {
            POPULATE_FPGA(6_Controller);
        } else {
            POPULATE_FPGA(6_Responder);
        }
    } else {
        throw std::runtime_error("Cannot create VMS for settings " + _vmsApplicationSettings->Subsystem);
    }
}

void FPGA::initialize() {
    SPDLOG_DEBUG("FPGA: initialize()");
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Initialize());
#endif
}

void FPGA::open() {
    SPDLOG_DEBUG("FPGA: open({},{},{})", _bitFile, _signature, _vmsApplicationSettings->RIO);
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Open(_bitFile, _signature,
                                                        _vmsApplicationSettings->RIO.c_str(), 0, &(session)));
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Download(session));
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Reset(session));
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Run(session, 0));
#endif
}

void FPGA::close() {
    SPDLOG_DEBUG("FPGA: close()");
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Close(session, 0));
#endif
}

void FPGA::finalize() {
    SPDLOG_DEBUG("FPGA: finalize()");
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Finalize());
#endif
}

float FPGA::chassisTemperature() {
#ifndef SIMULATOR
    uint64_t temperature;
    cRIO::NiThrowError(__PRETTY_FUNCTION__,
                       NiFpga_ReadU64(session, _chasisTemperatureResource, &temperature));
    return NiFpga_ConvertFromFxpToFloat(_chasisTemperatureTypeInfo, temperature);
#else
    return 7.7 + 2.0 * (static_cast<float>(random()) / RAND_MAX);
#endif
}

uint64_t FPGA::chassisTicks() {
#ifndef SIMULATOR
    uint64_t ticks;
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_ReadU64(session, _ticksResource, &ticks));
    return ticks;
#else
    static uint64_t ticks = 0;
    return ticks++;
#endif
}

void FPGA::setOperate(bool operate) {
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_WriteBool(session, _operateResource, operate));
#else
#endif
    Events::FPGAState::instance().setMisc(operate, false, false, false);
}

void FPGA::setPeriodOutputType(uint32_t period, int16_t outputType) {
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_WriteU32(session, _periodResource, period));
#endif
    Events::FPGAState::instance().setPeriodOutputType(period, outputType);
}

bool FPGA::ready() {
#ifndef SIMULATOR
    NiFpga_Bool ret = false;
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_ReadBool(session, _readyResource, &ret));
    return ret;
#else
    return true;
#endif
}

bool FPGA::timeouted() {
#ifndef SIMULATOR
    NiFpga_Bool ret = false;
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_ReadBool(session, _timeoutedResource, &ret));
    return ret;
#else
    return false;
#endif
}

bool FPGA::stopped() {
#ifndef SIMULATOR
    NiFpga_Bool ret = false;
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_ReadBool(session, _stoppedResource, &ret));
    return ret;
#else
    return false;
#endif
}

bool FPGA::FIFOFull() {
#ifndef SIMULATOR
    NiFpga_Bool ret = false;
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_ReadBool(session, _fifoFullResource, &ret));
    return ret;
#else
    return false;
#endif
}

void FPGA::readResponseFIFOs(uint32_t *data, size_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: readResponseFIFO({}, {})", length, timeoutInMs);
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__,
                       NiFpga_ReadFifoU32(session, _averageFIFO, data, length, timeoutInMs, &remaining));
// enable this if you are looking for raw, at source accelerometers data
#if 0
    size_t i = length;
    for (i = 0; i < length; i++) {
        if (data[i] != 0) {
            SPDLOG_INFO("readResponseFIFO {} {:.12f}", i, data[i]);
            break;
        }
    }
    if (i == length) {
        SPDLOG_INFO("No data");
    }
#endif
#else
    static long count = 0;
    static auto start = std::chrono::steady_clock::now();
    size_t channels = _channels * 3;
    int period = Events::FPGAState::instance().getPeriod();
    for (size_t i = 0; i < length; i += channels) {
        double cv = (2 * M_PI * period * static_cast<double>(count++)) / 1000.0f;
        // data are produced at Events::FPGAState::instance().getPeriod() ms period
        // scales target frequency into 0..2pi sin/cos period
        auto frequency_to_period = [cv](double frequency) { return (cv * frequency); };
        for (size_t j = 0; j < channels; j++) {
            size_t ch_index = i + j;
            // introduce periodic signals with frequencies of 61.3, 50.2 +
            // channel, 30.9 - channel and 17.5  Hz
            double pv = 1.5 * cos(frequency_to_period(61.3)) +
                        ((20 + j) / 10.0) * sin(frequency_to_period(50 + j)) +
                        ((channels + 20 - j) / 10.0) * sin(frequency_to_period(30 - j)) +
                        3 * cos(frequency_to_period(17.5));
            float d = ((0.0002 * static_cast<double>(random()) / RAND_MAX) - 0.0001) + pv / 10.0;
            uint64_t fxp = NiFpga_ConvertFromFloatToFxp(ResponseFxpTypeInfo, d);
            data[ch_index] = fxp;
#if 0
            std::cout << pv << " " << count << " ch_index " << ch_index << " " << fxp << " " << d << " "
                      << data[ch_index] << " " << Events::FPGAState::instance().getPeriod() << " "
                      << frequency_to_period(200) << std::endl;
#endif
        }
        start += std::chrono::milliseconds(period);
        std::this_thread::sleep_until(start);
    }
#endif
}
