/*
 * Accelerometer.h
 *
 *  Created on: Nov 1, 2017
 *      Author: ccontaxis
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <IAccelerometer.h>
#include <DataTypes.h>

struct vms_M1M3C;
struct vms_M2C;
struct vms_TMAC;

namespace LSST {
namespace VMS {

class IPublisher;
class IFPGA;
class VMSApplicationSettings;

class Accelerometer: public IAccelerometer {
private:
	IPublisher* publisher;
	IFPGA* fpga;
	VMSApplicationSettings* vmsApplicationSettings;

	vms_M1M3C* m1m3Data;
	vms_M2C* m2Data;
	vms_TMAC* tmaData;

	uint64_t u64Buffer[1024];
	float sglBuffer[2048];

public:
	Accelerometer(IPublisher* publisher, IFPGA* fpga, VMSApplicationSettings* vmsApplicationSettings);

	void enableAccelerometers();
	void disableAccelerometers();

	void sampleData();

private:
	void processM1M3();
	void processM2();
	void processTMA();
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* ACCELEROMETER_H_ */
