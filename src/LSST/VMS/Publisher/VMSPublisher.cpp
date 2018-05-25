/*
 * VMSPublisher.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#include <VMSPublisher.h>
#include <SAL_vms.h>
#include <Log.h>

namespace LSST {
namespace VMS {

VMSPublisher::VMSPublisher(SAL_vms* vmsSAL) {
	Log.Debug("VMSPublisher::VMSPublisher()");
	this->vmsSAL = vmsSAL;

	this->vmsSAL->salTelemetryPub((char*)"vms_M1M3");
	this->vmsSAL->salTelemetryPub((char*)"vms_M2Ms");
	this->vmsSAL->salTelemetryPub((char*)"vms_MTMount");
}

double VMSPublisher::getTimestamp() { return this->vmsSAL->getCurrentTime(); }

void VMSPublisher::putM1M3() { this->vmsSAL->putSample_M1M3(&this->m1m3); }
void VMSPublisher::putM2() { this->vmsSAL->putSample_M2(&this->m2ms); }
void VMSPublisher::putTMA() { this->vmsSAL->putSample_TMA(&this->mtMount); }

} /* namespace VMS */
} /* namespace LSST */
