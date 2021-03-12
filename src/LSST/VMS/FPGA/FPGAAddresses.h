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
    Accelerometer1X = 1,  // Not Used
    Accelerometer1Y = 2,  // Not Used
    Accelerometer1Z = 3,  // Not Used
    Accelerometer2X = 4,  // Not Used
    Accelerometer2Y = 5,  // Not Used
    Accelerometer2Z = 6,  // Not Used
    Accelerometer3X = 7,  // Not Used
    Accelerometer3Y = 8,  // Not Used
    Accelerometer3Z = 9,  // Not Used
    Accelerometer4X = 10, // Not Used
    Accelerometer4Y = 11, // Not Used
    Accelerometer4Z = 12, // Not Used
    Accelerometer5X = 13, // Not Used
    Accelerometer5Y = 14, // Not Used
    Accelerometer5Z = 15, // Not Used
    Accelerometer6X = 16, // Not Used
    Accelerometer6Y = 17, // Not Used
    Accelerometer6Z = 18, // Not Used
    Sync1 = 19,           // Not Used
    Sync2 = 20,           // Not Used
    Sync3 = 21,           // Not Used
    Sync4 = 22,           // Not Used
    Accelerometer1 = 23,  // Not Used
    Accelerometer2 = 24,  // Not Used
    Accelerometer3 = 25,  // Not Used
    Accelerometer4 = 26,  // Not Used
    Accelerometer5 = 27,  // Not Used
    Accelerometer6 = 28,  // Not Used
    Accelerometers = 29,
    Timestamp = 30,   // Not Used
    SamplePeriod = 31 // Not Used
  };
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* FPGAADDRESSES_H_ */
