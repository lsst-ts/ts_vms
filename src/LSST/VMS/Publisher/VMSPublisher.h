/*
 * VMSPublisher.h
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#ifndef VMSPUBLISHER_H_
#define VMSPUBLISHER_H_

#include <IPublisher.h>
#include <SAL_vmsC.h>

class SAL_vms;

namespace LSST {
namespace VMS {

class VMSPublisher : public IPublisher {
private:
	SAL_vms* vmsSAL;

	vms_M1M3C m1m3;
	vms_M2C m2ms;
	vms_TMAC mtMount;

public:
	VMSPublisher(SAL_vms* vmsSAL);

	vms_M1M3C* getM1M3() { return &this->m1m3; }
	vms_M2C* getM2() { return &this->m2ms; }
	vms_TMAC* getTMA() { return &this->mtMount; }

	double getTimestamp();

	void putM1M3();
	void putM2();
	void putTMA();
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* VMSPUBLISHER_H_ */
