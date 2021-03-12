/*
 * IFPGA.cpp
 *
 *  Created on: Oct 2, 2017
 *      Author: ccontaxis
 */

#include <IFPGA.h>

namespace LSST {
namespace VMS {

IFPGA::~IFPGA() {}

int32_t IFPGA::initialize() { return 0; }
int32_t IFPGA::open() { return 0; }
int32_t IFPGA::close() { return 0; }
int32_t IFPGA::finalize() { return 0; }

bool IFPGA::isErrorCode(int32_t status) { return false; }

int32_t IFPGA::setTimestamp(double timestamp) { return 0; }

int32_t IFPGA::waitForOuterLoopClock(int32_t timeout) { return 0; }
int32_t IFPGA::ackOuterLoopClock() { return 0; }

int32_t IFPGA::writeCommandFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs) { return 0; }
int32_t IFPGA::writeRequestFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs) { return 0; }
int32_t IFPGA::writeRequestFIFO(uint16_t data, int32_t timeoutInMs) { return 0; }
int32_t IFPGA::readU64ResponseFIFO(uint64_t *data, int32_t length, int32_t timeoutInMs) { return 0; }
int32_t IFPGA::readSGLResponseFIFO(float *data, int32_t length, int32_t timeoutInMs) { return 0; }

} /* namespace VMS */
} /* namespace LSST */
