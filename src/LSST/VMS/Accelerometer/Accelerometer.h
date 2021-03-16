/*
 * Accelerometer.h
 *
 *  Created on: Nov 1, 2017
 *      Author: ccontaxis
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <DataTypes.h>
#include <IAccelerometer.h>
#include <SAL_MTVMSC.h>

struct MTVMS_m1m3C;
struct MTVMS_m2C;
struct MTVMS_tmaC;

namespace LSST {
namespace VMS {

class IFPGA;
class VMSApplicationSettings;

class Accelerometer : public IAccelerometer {
public:
    Accelerometer(IFPGA *_fpga, VMSApplicationSettings *vmsApplicationSettings);

    void enableAccelerometers();
    void disableAccelerometers();

    void sampleData();

private:
    void processM1M3();
    void processM2();
    void processTMA();

    IFPGA *fpga;
    VMSApplicationSettings *vmsApplicationSettings;

    MTVMS_m1m3C *m1m3Data;
    MTVMS_m2C *m2Data;
    MTVMS_tmaC *tmaData;

    uint64_t u64Buffer[1024];
    float sglBuffer[2048];
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* ACCELEROMETER_H_ */
