/*
 * FPGA.cpp
 *
 *  Created on: Sep 28, 2017
 *      Author: ccontaxis
 */

#include <FPGA.h>
#include <NiFpga_VMS_3_Master.h>
#include <NiFpga_VMS_3_Slave.h>
#include <NiFpga_VMS_6_Master.h>
#include <NiFpga_VMS_6_Slave.h>
#include <Timestamp.h>
#include <FPGAAddresses.h>
#include <VMSApplicationSettings.h>
#include <unistd.h>

namespace LSST {
namespace VMS {

FPGA::FPGA(VMSApplicationSettings* vmsApplicationSettings) {
	this->vmsApplicationSettings = vmsApplicationSettings;
	this->session = 0;
	this->remaining = 0;
	this->outerLoopIRQContext = 0;
	if(this->vmsApplicationSettings->IsMaster) {
		if (this->vmsApplicationSettings->NumberOfSensors == 3) {
			this->mode = 0;
		}
		else {
			this->mode = 2;
		}
	}
	else {
		if (this->vmsApplicationSettings->NumberOfSensors == 3) {
			this->mode = 1;
		}
		else {
			this->mode = 3;
		}
	}
	this->bitFile = this->getBitFile();
	this->signature = this->getSignature();
	this->commandFIFO = this->getCommandFIFO();
	this->requestFIFO = this->getRequestFIFO();
	this->u64ResponseFIFO = this->getU64ResponseFIFO();
	this->sglResponseFIFO = this->getSGLResponseFIFO();
	this->buffer[0] = 0;
}

int32_t FPGA::initialize() {
	return NiFpga_Initialize();
}

int32_t FPGA::open() {
	int32_t status = NiFpga_Open(this->bitFile, this->signature, "RIO0", 0, &(this->session));
	status = NiFpga_Abort(this->session);
	status = NiFpga_Download(this->session);
	status = NiFpga_Reset(this->session);
	status = NiFpga_Run(this->session, 0);
	usleep(1000000);
	NiFpga_ReserveIrqContext(this->session, &this->outerLoopIRQContext);
	return status;
}

int32_t FPGA::close() {
	NiFpga_UnreserveIrqContext(this->session, this->outerLoopIRQContext);
	return NiFpga_Close(this->session, 0);
}

int32_t FPGA::finalize() {
	return NiFpga_Finalize();
}

bool FPGA::isErrorCode(int32_t status) {
	return NiFpga_IsError(status);
}

int32_t FPGA::setTimestamp(double timestamp) {
	uint64_t raw = Timestamp::toRaw(timestamp);
	uint16_t buffer[5];
	buffer[0] = FPGAAddresses::Timestamp;
	buffer[1] = (raw >> 48) & 0xFFFF;
	buffer[2] = (raw >> 32) & 0xFFFF;
	buffer[3] = (raw >> 16) & 0xFFFF;
	buffer[4] = (raw >> 0) & 0xFFFF;
	return this->writeCommandFIFO(buffer, 5, 0);
}

int32_t FPGA::waitForOuterLoopClock(int32_t timeout) {
	uint32_t assertedIRQs = 0;
	uint8_t timedOut = false;
	int32_t result = NiFpga_WaitOnIrqs(this->session, this->outerLoopIRQContext, NiFpga_Irq_0, timeout, &assertedIRQs, &timedOut);
	return result;
}

int32_t FPGA::ackOuterLoopClock() {
	return NiFpga_AcknowledgeIrqs(this->session, NiFpga_Irq_0);
}

int32_t FPGA::writeCommandFIFO(uint16_t* data, int32_t length, int32_t timeoutInMs) {
	return NiFpga_WriteFifoU16(this->session, this->commandFIFO, data, length, timeoutInMs, &this->remaining);
}

int32_t FPGA::writeRequestFIFO(uint16_t* data, int32_t length, int32_t timeoutInMs) {
	return NiFpga_WriteFifoU16(this->session, this->requestFIFO, data, length, timeoutInMs, &this->remaining);
}

int32_t FPGA::writeRequestFIFO(uint16_t data, int32_t timeoutInMs) {
	this->buffer[0] = data;
	return this->writeRequestFIFO(this->buffer, 1, timeoutInMs);
}

int32_t FPGA::readU64ResponseFIFO(uint64_t* data, int32_t length, int32_t timeoutInMs) {
	return NiFpga_ReadFifoU64(this->session, this->u64ResponseFIFO, data, length, timeoutInMs, &this->remaining);
}

int32_t FPGA::readSGLResponseFIFO(float* data, int32_t length, int32_t timeoutInMs) {
	return NiFpga_ReadFifoSgl(this->session, this->sglResponseFIFO, data, length, timeoutInMs, &this->remaining);
}

char* FPGA::getBitFile() {
	switch(this->mode) {
	case 1: return (char*)"/usr/ts_VMS/"NiFpga_VMS_3_Slave_Bitfile;
	case 2: return (char*)"/usr/ts_VMS/"NiFpga_VMS_6_Master_Bitfile;
	case 3: return (char*)"/usr/ts_VMS/"NiFpga_VMS_6_Slave_Bitfile;
	default: return (char*)"/usr/ts_VMS/"NiFpga_VMS_3_Master_Bitfile;
	}
}

const char* FPGA::getSignature() {
	switch(this->mode) {
	case 1: return NiFpga_VMS_3_Slave_Signature;
	case 2: return NiFpga_VMS_6_Master_Signature;
	case 3: return NiFpga_VMS_6_Slave_Signature;
	default: return NiFpga_VMS_3_Master_Signature;
	}
}

uint32_t FPGA::getCommandFIFO() {
	switch(this->mode) {
	case 1: return NiFpga_VMS_3_Slave_HostToTargetFifoU16_CommandFIFO;
	case 2: return NiFpga_VMS_6_Master_HostToTargetFifoU16_CommandFIFO;
	case 3: return NiFpga_VMS_6_Slave_HostToTargetFifoU16_CommandFIFO;
	default: return NiFpga_VMS_3_Master_HostToTargetFifoU16_CommandFIFO;
	}
}

uint32_t FPGA::getRequestFIFO() {
	switch(this->mode) {
	case 1: return NiFpga_VMS_3_Slave_HostToTargetFifoU16_RequestFIFO;
	case 2: return NiFpga_VMS_6_Master_HostToTargetFifoU16_RequestFIFO;
	case 3: return NiFpga_VMS_6_Slave_HostToTargetFifoU16_RequestFIFO;
	default: return NiFpga_VMS_3_Master_HostToTargetFifoU16_RequestFIFO;
	}
}

uint32_t FPGA::getU64ResponseFIFO() {
	switch(this->mode) {
	case 1: return NiFpga_VMS_3_Slave_TargetToHostFifoU64_U64ResponseFIFO;
	case 2: return NiFpga_VMS_6_Master_TargetToHostFifoU64_U64ResponseFIFO;
	case 3: return NiFpga_VMS_6_Slave_TargetToHostFifoU64_U64ResponseFIFO;
	default: return NiFpga_VMS_3_Master_TargetToHostFifoU64_U64ResponseFIFO;
	}
}

uint32_t FPGA::getSGLResponseFIFO() {
	switch(this->mode) {
	case 1: return NiFpga_VMS_3_Slave_TargetToHostFifoSgl_SGLResponseFIFO;
	case 2: return NiFpga_VMS_6_Master_TargetToHostFifoSgl_SGLResponseFIFO;
	case 3: return NiFpga_VMS_6_Slave_TargetToHostFifoSgl_SGLResponseFIFO;
	default: return NiFpga_VMS_3_Master_TargetToHostFifoSgl_SGLResponseFIFO;
	}
}

} /* namespace VMS */
} /* namespace LSST */
