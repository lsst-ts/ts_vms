/*
 * Accelerometer.h
 *
 *  Created on: Nov 1, 2017
 *      Author: ccontaxis
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <DataTypes.h>
#include <FPGA.h>
#include <SAL_MTVMS.h>
#include <VMSApplicationSettings.h>

namespace LSST {
namespace VMS {

/**
 * VMS Accelerometer sampling.
 */
class Accelerometer {
public:
    Accelerometer(FPGA *_fpga, VMSApplicationSettings *vmsApplicationSettings);

    void enableAccelerometers(uint32_t period, int16_t outputType);
    void disableAccelerometers();

    void sampleData();

private:
    FPGA *fpga;

    enum { M1M3, M2, CameraRotator, TMA } subsystem;

    int numberOfSensors;
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* ACCELEROMETER_H_ */
