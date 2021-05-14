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
#include <NiFpga_VMS_3_Master.h>
#include <NiFpga_VMS_3_Slave.h>
#include <NiFpga_VMS_6_Master.h>
#include <NiFpga_VMS_6_Slave.h>
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

#define POPULATE_FPGA(type)                                                     \
    _bitFile = "/var/lib/ts-VMS/" NiFpga_VMS_##type##_Bitfile;                  \
    _signature = NiFpga_VMS_##type##_Signature;                                 \
    _commandFIFO = NiFpga_VMS_##type##_HostToTargetFifoU16_CommandFIFO;         \
    _requestFIFO = NiFpga_VMS_##type##_HostToTargetFifoU16_RequestFIFO;         \
    _u64ResponseFIFO = NiFpga_VMS_##type##_TargetToHostFifoU64_U64ResponseFIFO; \
    _sglResponseFIFO = NiFpga_VMS_##type##_TargetToHostFifoSgl_SGLResponseFIFO;

FPGA::FPGA(VMSApplicationSettings *vmsApplicationSettings) {
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
        POPULATE_FPGA(6_Master);
    } else if (_vmsApplicationSettings->Subsystem == "M1M3") {
        _channels = 3;
        POPULATE_FPGA(3_Master);
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

void FPGA::setTimestamp(double timestamp) {
    SPDLOG_TRACE("FPGA: setTimestamp({})", timestamp);
    uint64_t raw = Timestamp::toRaw(timestamp);
    uint16_t buffer[5];
    buffer[0] = FPGAAddresses::Timestamp;
    // TODO this isn't low or big endian, this is mess. Should be rewritten to
    // probably big endian (network standard), but need changes in FPGA
    // (LabView has various bite shifting end ordering blocks, which should be
    // used instead of self-wired routines). When done, the code can be simplified to:
    // raw = htobe64(raw);
    // memcpy(buffer + 1, &raw, 8);
    buffer[1] = (raw >> 48) & 0xFFFF;
    buffer[2] = (raw >> 32) & 0xFFFF;
    buffer[3] = (raw >> 16) & 0xFFFF;
    buffer[4] = (raw >> 0) & 0xFFFF;
    writeCommandFIFO(buffer, 5, 0);
}

float FPGA::chasisTemperature() {
    writeRequestFIFO(FPGAAddresses::ChasisTemperature, 0);
    uint64_t buffer;
    readU64ResponseFIFO(&buffer, 1, 500);
    return static_cast<float>(buffer) / 100.0;
}

void FPGA::waitForOuterLoopClock(int32_t timeout) {
    SPDLOG_TRACE("FPGA: waitForOuterLoopClock({})", timeout);
#ifndef SIMULATOR
    uint32_t assertedIRQs = 0;
    uint8_t timedOut = false;
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_WaitOnIrqs(session, outerLoopIRQContext, NiFpga_Irq_0,
                                                              timeout, &assertedIRQs, &timedOut));
#endif
}

void FPGA::ackOuterLoopClock() {
    SPDLOG_TRACE("FPGA: ackOuterLoopClock()");
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__, NiFpga_AcknowledgeIrqs(session, NiFpga_Irq_0));
#endif
}

void FPGA::writeCommandFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: writeCommandFIFO({})", length);
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__,
                       NiFpga_WriteFifoU16(session, _commandFIFO, data, length, timeoutInMs, &remaining));
#endif
}

void FPGA::writeRequestFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: writeRequestFIFO(Length = {})", length);
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__,
                       NiFpga_WriteFifoU16(session, _requestFIFO, data, length, timeoutInMs, &remaining));
#endif
}

void FPGA::writeRequestFIFO(uint16_t data, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: writeRequestFIFO(Data = {})", data);
    writeRequestFIFO(&data, 1, timeoutInMs);
}

void FPGA::readU64ResponseFIFO(uint64_t *data, size_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: readU64ResponseFIFO({})", length);
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__,
                       NiFpga_ReadFifoU64(session, _u64ResponseFIFO, data, length, timeoutInMs, &remaining));
#else
    for (size_t i = 0; i < length; i++) {
        data[i] = Timestamp::toRaw(VMSPublisher::instance().getTimestamp());
    }
#endif
}

void FPGA::readSGLResponseFIFO(float *data, size_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: readSGLResponseFIFO({})", length);
#ifndef SIMULATOR
    cRIO::NiThrowError(__PRETTY_FUNCTION__,
                       NiFpga_ReadFifoSgl(session, _sglResponseFIFO, data, length, timeoutInMs, &remaining));
// enable this if you are looking for raw, at source accelerometers data
#if 0
    size_t i = length;
    for (i = 0; i < length; i++) {
        if (data[i] != 0) {
            SPDLOG_INFO("readSGLResponseFIFO {} {:.12f}", i, data[i]);
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
            data[ch] = (((50.0 * static_cast<double>(random()) / RAND_MAX) - 25.0) + pv) / 1000000.0;
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
