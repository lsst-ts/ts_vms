/*
 * FPGA.h
 *
 *  Created on: Sep 28, 2017
 *      Author: ccontaxis
 */

#ifndef FPGA_H_
#define FPGA_H_

#include <NiFpga.h>

#include <cRIO/SimpleFPGA.h>

namespace LSST {
namespace VMS {

class VMSApplicationSettings;

/*!
 * The class used to communicate with the FPGA.
 */
class FPGA : public LSST::cRIO::SimpleFPGA {
public:
    FPGA(VMSApplicationSettings *vmsApplicationSettings);

    void initialize() override;
    void open() override;
    void close() override;
    void finalize() override;

    void setTimestamp(double timestamp);
    float chasisTemperature();

    void waitForOuterLoopClock(int32_t timeout);
    void ackOuterLoopClock();

    void writeCommandFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs);
    void writeRequestFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs);
    void writeRequestFIFO(uint16_t data, int32_t timeoutInMs);
    void readU64ResponseFIFO(uint64_t *data, size_t length, int32_t timeoutInMs);
    void readSGLResponseFIFO(float *data, size_t length, int32_t timeoutInMs);

private:
    VMSApplicationSettings *_vmsApplicationSettings;
    uint32_t session;
    size_t remaining;
    NiFpga_IrqContext outerLoopIRQContext;
    uint8_t _channels;
    const char *_bitFile;
    const char *_signature;
    uint32_t _commandFIFO;
    uint32_t _requestFIFO;
    uint32_t _u64ResponseFIFO;
    uint32_t _sglResponseFIFO;
};

}  // namespace VMS
}  // namespace LSST

#endif /* FPGA_H_ */
