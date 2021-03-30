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

struct MTVMS_m1m3C;
struct MTVMS_m2C;
struct MTVMS_tmaC;

namespace LSST {
namespace VMS {

class VMSApplicationSettings;

class Accelerometer {
public:
    Accelerometer(FPGA *_fpga, VMSApplicationSettings *vmsApplicationSettings);

    void enableAccelerometers();
    void disableAccelerometers();

    void sampleData();

private:
    void processM1M3();
    void processM2();
    void processCameraRotator();
    void processTMA();

    FPGA *fpga;

    enum { M1M3, M2, CameraRotator, TMA } subsystem;

    int numberOfSensors;

    MTVMS_m1m3C *m1m3Data;
    MTVMS_m2C *m2Data;
    MTVMS_cameraRotatorC *cameraRotatorData;
    // MTVMS_tmaC *tmaData;

    uint64_t u64Buffer[1024];
    float sglBuffer[2048];
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* ACCELEROMETER_H_ */
