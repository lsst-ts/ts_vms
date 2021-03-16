/*
 * FPGA.h
 *
 *  Created on: Sep 28, 2017
 *      Author: ccontaxis
 */

#ifndef FPGA_H_
#define FPGA_H_

#include <NiFpga.h>

namespace LSST {
namespace VMS {

class VMSApplicationSettings;

/*!
 * The class used to communicate with the FPGA.
 */
class FPGA {
private:
    VMSApplicationSettings *vmsApplicationSettings;
    uint32_t session;
    size_t remaining;
    NiFpga_IrqContext outerLoopIRQContext;
    int32_t mode;
    char *bitFile;
    const char *signature;
    uint32_t commandFIFO;
    uint32_t requestFIFO;
    uint32_t u64ResponseFIFO;
    uint32_t sglResponseFIFO;
    uint16_t buffer[1];

public:
    FPGA(VMSApplicationSettings *vmsApplicationSettings);

    int32_t initialize();
    int32_t open();
    int32_t close();
    int32_t finalize();

    bool isErrorCode(int32_t status);

    int32_t setTimestamp(double timestamp);

    int32_t waitForOuterLoopClock(int32_t timeout);
    int32_t ackOuterLoopClock();

    int32_t writeCommandFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs);
    int32_t writeRequestFIFO(uint16_t *data, int32_t length, int32_t timeoutInMs);
    int32_t writeRequestFIFO(uint16_t data, int32_t timeoutInMs);
    int32_t readU64ResponseFIFO(uint64_t *data, size_t length, int32_t timeoutInMs);
    int32_t readSGLResponseFIFO(float *data, size_t length, int32_t timeoutInMs);

private:
    char *getBitFile();
    const char *getSignature();
    uint32_t getCommandFIFO();
    uint32_t getRequestFIFO();
    uint32_t getU64ResponseFIFO();
    uint32_t getSGLResponseFIFO();
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* FPGA_H_ */
