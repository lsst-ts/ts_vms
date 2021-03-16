/*
 * FPGA.cpp
 *
 *  Created on: Sep 28, 2017
 *      Author: ccontaxis
 */

#include <FPGA.h>
#include <FPGAAddresses.h>
#include <spdlog/spdlog.h>
#include <NiFpga_VMS_3_Master.h>
#include <NiFpga_VMS_3_Slave.h>
#include <NiFpga_VMS_6_Master.h>
#include <NiFpga_VMS_6_Slave.h>
#include <Timestamp.h>
#include <VMSApplicationSettings.h>
#include <VMSPublisher.h>
#include <unistd.h>
#include <endian.h>

namespace LSST {
namespace VMS {

FPGA::FPGA(VMSApplicationSettings *vmsApplicationSettings) {
    SPDLOG_DEBUG("FPGA::FPGA()");
    vmsApplicationSettings = vmsApplicationSettings;
    session = 0;
    remaining = 0;
    outerLoopIRQContext = 0;
    if (vmsApplicationSettings->IsMaster) {
        if (vmsApplicationSettings->NumberOfSensors == 3) {
            mode = 0;
        } else {
            mode = 2;
        }
    } else {
        if (vmsApplicationSettings->NumberOfSensors == 3) {
            mode = 1;
        } else {
            mode = 3;
        }
    }
    bitFile = getBitFile();
    signature = getSignature();
    commandFIFO = getCommandFIFO();
    requestFIFO = getRequestFIFO();
    u64ResponseFIFO = getU64ResponseFIFO();
    sglResponseFIFO = getSGLResponseFIFO();
    buffer[0] = 0;
}

int32_t FPGA::initialize() {
    SPDLOG_DEBUG("FPGA: initialize()");
    return NiFpga_Initialize();
}

int32_t FPGA::open() {
    SPDLOG_DEBUG("FPGA: open()");
    int32_t status = NiFpga_Open(bitFile, signature, "RIO0", 0, &(session));
    status = NiFpga_Abort(session);
    status = NiFpga_Download(session);
    status = NiFpga_Reset(session);
    status = NiFpga_Run(session, 0);
    usleep(1000000);
    NiFpga_ReserveIrqContext(session, &outerLoopIRQContext);
    return status;
}

int32_t FPGA::close() {
    SPDLOG_DEBUG("FPGA: close()");
    NiFpga_UnreserveIrqContext(session, outerLoopIRQContext);
    return NiFpga_Close(session, 0);
}

int32_t FPGA::finalize() {
    SPDLOG_DEBUG("FPGA: finalize()");
    return NiFpga_Finalize();
}

bool FPGA::isErrorCode(int32_t status) {
    SPDLOG_DEBUG("FPGA: isErrorCode({})", status);
    return NiFpga_IsError(status);
}

int32_t FPGA::setTimestamp(double timestamp) {
    SPDLOG_TRACE("FPGA: setTimestamp({})", timestamp);
    uint64_t raw = Timestamp::toRaw(timestamp);
    uint16_t buffer[5];
    buffer[0] = FPGAAddresses::Timestamp;
    // TODO this isn't low or big endian, this is mess. Should be rewritten to
    // probably big endian (network standard), but need changes in FPGA
    // When done, the code can be simplified to:
    // raw = htobe64(raw);
    // memcpy(buffer + 1, &raw, 8);
    buffer[1] = (raw >> 48) & 0xFFFF;
    buffer[2] = (raw >> 32) & 0xFFFF;
    buffer[3] = (raw >> 16) & 0xFFFF;
    buffer[4] = (raw >> 0) & 0xFFFF;
    return writeCommandFIFO(buffer, 5, 0);
}

int32_t FPGA::waitForOuterLoopClock(int32_t timeout) {
    SPDLOG_TRACE("FPGA: waitForOuterLoopClock({})", timeout);
    uint32_t assertedIRQs = 0;
    uint8_t timedOut = false;
    int32_t result =
            NiFpga_WaitOnIrqs(session, outerLoopIRQContext, NiFpga_Irq_0, timeout, &assertedIRQs, &timedOut);
    return result;
}

int32_t FPGA::ackOuterLoopClock() {
    SPDLOG_TRACE("FPGA: ackOuterLoopClock()");
    return NiFpga_AcknowledgeIrqs(session, NiFpga_Irq_0);
}

int32_t FPGA::writeCommandFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: writeCommandFIFO({})", length);
#ifndef SIMULATOR
    return NiFpga_WriteFifoU16(session, commandFIFO, data, length, timeoutInMs, &remaining);
#else
    return length;
#endif
}

int32_t FPGA::writeRequestFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: writeRequestFIFO(Length = {})", length);
#ifndef SIMULATOR
    return NiFpga_WriteFifoU16(session, requestFIFO, data, length, timeoutInMs, &remaining);
#else
    return length;
#endif
}

int32_t FPGA::writeRequestFIFO(uint16_t data, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: writeRequestFIFO(Data = {})", data);
    buffer[0] = data;
    return writeRequestFIFO(buffer, 1, timeoutInMs);
}

int32_t FPGA::readU64ResponseFIFO(uint64_t *data, int32_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: readU64ResponseFIFO({})", length);
#ifndef SIMULATOR
    return NiFpga_ReadFifoU64(session, u64ResponseFIFO, data, length, timeoutInMs, &remaining);
#else
    *data = Timestamp::toRaw(VMSPublisher::instance().getTimestamp());
#endif
}

int32_t FPGA::readSGLResponseFIFO(float *data, int32_t length, int32_t timeoutInMs) {
    SPDLOG_TRACE("FPGA: readSGLResponseFIFO({})", length);
    return NiFpga_ReadFifoSgl(session, sglResponseFIFO, data, length, timeoutInMs, &remaining);
}

char *FPGA::getBitFile() {
    switch (mode) {
        case 1:
            return (char *)"/usr/ts_VMS/" NiFpga_VMS_3_Slave_Bitfile;
        case 2:
            return (char *)"/usr/ts_VMS/" NiFpga_VMS_6_Master_Bitfile;
        case 3:
            return (char *)"/usr/ts_VMS/" NiFpga_VMS_6_Slave_Bitfile;
        default:
            return (char *)"/usr/ts_VMS/" NiFpga_VMS_3_Master_Bitfile;
    }
}

const char *FPGA::getSignature() {
    switch (mode) {
        case 1:
            return NiFpga_VMS_3_Slave_Signature;
        case 2:
            return NiFpga_VMS_6_Master_Signature;
        case 3:
            return NiFpga_VMS_6_Slave_Signature;
        default:
            return NiFpga_VMS_3_Master_Signature;
    }
}

uint32_t FPGA::getCommandFIFO() {
    switch (mode) {
        case 1:
            return NiFpga_VMS_3_Slave_HostToTargetFifoU16_CommandFIFO;
        case 2:
            return NiFpga_VMS_6_Master_HostToTargetFifoU16_CommandFIFO;
        case 3:
            return NiFpga_VMS_6_Slave_HostToTargetFifoU16_CommandFIFO;
        default:
            return NiFpga_VMS_3_Master_HostToTargetFifoU16_CommandFIFO;
    }
}

uint32_t FPGA::getRequestFIFO() {
    switch (mode) {
        case 1:
            return NiFpga_VMS_3_Slave_HostToTargetFifoU16_RequestFIFO;
        case 2:
            return NiFpga_VMS_6_Master_HostToTargetFifoU16_RequestFIFO;
        case 3:
            return NiFpga_VMS_6_Slave_HostToTargetFifoU16_RequestFIFO;
        default:
            return NiFpga_VMS_3_Master_HostToTargetFifoU16_RequestFIFO;
    }
}

uint32_t FPGA::getU64ResponseFIFO() {
    switch (mode) {
        case 1:
            return NiFpga_VMS_3_Slave_TargetToHostFifoU64_U64ResponseFIFO;
        case 2:
            return NiFpga_VMS_6_Master_TargetToHostFifoU64_U64ResponseFIFO;
        case 3:
            return NiFpga_VMS_6_Slave_TargetToHostFifoU64_U64ResponseFIFO;
        default:
            return NiFpga_VMS_3_Master_TargetToHostFifoU64_U64ResponseFIFO;
    }
}

uint32_t FPGA::getSGLResponseFIFO() {
    switch (mode) {
        case 1:
            return NiFpga_VMS_3_Slave_TargetToHostFifoSgl_SGLResponseFIFO;
        case 2:
            return NiFpga_VMS_6_Master_TargetToHostFifoSgl_SGLResponseFIFO;
        case 3:
            return NiFpga_VMS_6_Slave_TargetToHostFifoSgl_SGLResponseFIFO;
        default:
            return NiFpga_VMS_3_Master_TargetToHostFifoSgl_SGLResponseFIFO;
    }
}

} /* namespace VMS */
} /* namespace LSST */
