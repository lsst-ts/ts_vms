/*
 * FPGAAddresses.h
 *
 *  Created on: Oct 10, 2017
 *      Author: ccontaxis
 */

#ifndef FPGAADDRESSES_H_
#define FPGAADDRESSES_H_

namespace LSST {
namespace VMS {

/*!
 * An enumeration match FPGA signals to port numbers.
 */
struct FPGAAddresses {
    enum Type {
        NA = 0,
        Accelerometers = 1,
        Timestamp = 2,
        ChasisTemperature = 3,
    };
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* FPGAADDRESSES_H_ */
