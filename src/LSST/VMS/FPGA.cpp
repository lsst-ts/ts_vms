/*
 * FPGA.cpp
 *
 *  Created on: Sep 28, 2017
 *      Author: ccontaxis
 */

#include <cRIO/NiError.h>
#include <FPGA.h>
#include <FPGAAddresses.h>
#include <spdlog/spdlog.h>
#include <NiFpga_VMS_3_Controller.h>
#include <NiFpga_VMS_3_Responder.h>
#include <NiFpga_VMS_6_Controller.h>
#include <NiFpga_VMS_6_Responder.h>
#include <NiFpga_VMS_CameraRotator.h>
#include <Timestamp.h>
#include <VMSApplicationSettings.h>
#include <VMSPublisher.h>
#include <unistd.h>
#include <chrono>

#ifdef SIMULATOR
#include <math.h>
#endif

namespace LSST {
namespace VMS {

FPGA::FPGA(token) : SimpleFPGA(LSST::cRIO::VMS) {}

#define NiFpga_VMS_CameraRotator_ControlBool_Operate -1
#define NiFpga_VMS_6_Responder_ControlBool_Operate -1
#define NiFpga_VMS_3_Responder_ControlBool_Operate -1

#define POPULATE_FPGA(type)                                                                    \
    _bitFile = "/var/lib/MTVMS/" NiFpga_VMS_##type##_Bitfile;                                  \
    _signature = NiFpga_VMS_##type##_Signature;                                                \
    _responseFIFO = NiFpga_VMS_##type##_TargetToHostFifoU32_ResponseFIFO;                      \
    _chasisTemperatureResource = NiFpga_VMS_##type##_IndicatorFxp_ChassisTemperature_Resource; \
    _chasisTemperatureTypeInfo = NiFpga_VMS_##type##_IndicatorFxp_ChassisTemperature_TypeInfo; \
    _operateResource = NiFpga_VMS_##type##_ControlBool_Operate;                                \
    _periodResource = NiFpga_VMS_##type##_ControlU32_Periodms;                                 \
    _outputTypeResource = NiFpga_VMS_##type##_ControlI16_Outputtype;

void FPGA::populate(VMSApplicationSettings *vmsApplicationSettings) {
    SPDLOG_TRACE("FPGA::FPGA()");
    _vmsApplicationSettings = vmsApplicationSettings;
    session = 0;
    remaining = 0;
    outerLoopIRQContext = 0;
    if (_vmsApplicationSettings->Subsystem == "CameraRotator") {
        _channels = 3;
        POPULATE_FPGA(CameraRotator);
    } else if (_vmsApplicationSettings->Subsystem == "M2") {
        _channels = 6;
        if (_vmsApplicationSettings->IsController) {
            POPULATE_FPGA(6_Controller);
        } else {
            POPULATE_FPGA(6_Responder);
        }
    } else if (_vmsApplicationSettings->Subsystem == "M1M3") {
        _channels = 3;
        if (_vmsApplicationSettings->IsController) {
            POPULATE_FPGA(3_Controller);
        } else {
            POPULATE_FPGA(3_Responder);
        }
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
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Abort(session));
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Download(session));
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Reset(session));
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Run(session, 0));
    std::this_thread::sleep_for(std::chrono::seconds(_channels == 3 ? 1 : 3));
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_ReserveIrqContext(session, &outerLoopIRQContext));
#endif
}

void FPGA::close() {
    SPDLOG_DEBUG("FPGA: close()");
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_UnreserveIrqContext(session, outerLoopIRQContext));
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Close(session, 0));
#endif
}

void FPGA::finalize() {
    SPDLOG_DEBUG("FPGA: finalize()");
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_Finalize());
#endif
}

float FPGA::chasisTemperature() {
#ifndef SIMULATOR
    uint64_t temperature;
    cRIO::NiThrowError(__PRETTY_FUNCTION__,
                       NiFpga_ReadU64(session, _chasisTemperatureResource, &temperature));
    return NiFpga_ConvertFromFxpToDouble(_chasisTemperatureTypeInfo, temperature);
#else
    return 7.7;
#endif
}

void FPGA::setOperate(bool operate) {
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_WriteBool(session, _operateResource, operate));
#else
    // _ready = true;
#endif
}

void FPGA::setPeriod(uint32_t period) {
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_WriteU32(session, _periodResource, period));
#else
    // _ready = true;
#endif
}

void FPGA::setOutputType(int16_t outputType) {
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_WriteI16(session, _outputTypeResource, outputType));
#else
    // _ready = true;
#endif
}

void FPGA::readResponseFIFO(uint32_t *data, size_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: readResponseFIFO({})", length);
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__,
                       NiFpga_ReadFifoU32(session, _responseFIFO, data, length, timeoutInMs, &remaining));
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
    int channels = _channels * 3;
    for (size_t i = 0; i < length; i += channels) {
        double cv = M_PI * static_cast<double>(count++);
        // data are produced at 1kHz (see sleep_until) / 1 ms period
        // scales target frequency into 0..2pi sin/cos period
        auto frequency_to_period = [cv](double frequency) { return cv / (((1 / frequency) / 2) * 1000.0); };
        // introduce periodic signals with frequencies of 400, 200, 100, 50, 25 and 12.5 Hz
        double pv = 2.7 * sin(frequency_to_period(400)) + 6 * sin(frequency_to_period(200)) +
                    1.5 * cos(frequency_to_period(100)) + 2 * sin(frequency_to_period(50)) +
                    4 * sin(frequency_to_period(25)) + 3 * cos(frequency_to_period(12.5));
        for (size_t ch = i; ch < i + channels; ch++) {
            data[ch] = NiFpga_ConvertFromFloatToFxp(
                    ResponseFxpTypeInfo,
                    (((50.0 * static_cast<double>(random()) / RAND_MAX) - 25.0) + pv) / 1000000.0);
        }
        // high counts will be numerically unstable
        // limit must be integer multiple of frequencies introduced
        if (count == 40000) {
            count = 0;
        }
        start += std::chrono::microseconds(1000);
        std::this_thread::sleep_until(start);
    }
#endif
}

} /* namespace VMS */
} /* namespace LSST */
