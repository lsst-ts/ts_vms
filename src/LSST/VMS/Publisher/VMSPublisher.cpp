/*
 * VMSPublisher.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#include <Log.h>
#include <SAL_MTVMS.h>
#include <VMSPublisher.h>

namespace LSST {
namespace VMS {

VMSPublisher::VMSPublisher(std::shared_ptr<SAL_MTVMS> vmsSAL) {
  Log.Debug("VMSPublisher::VMSPublisher()");
  this->vmsSAL = vmsSAL;

  this->vmsSAL->salTelemetryPub((char *)"MTVMS_m1m3");
  this->vmsSAL->salTelemetryPub((char *)"MTVMS_m2");
  this->vmsSAL->salTelemetryPub((char *)"MTVMS_tma");
}

double VMSPublisher::getTimestamp() { return this->vmsSAL->getCurrentTime(); }

void VMSPublisher::putM1M3() { this->vmsSAL->putSample_m1m3(&this->m1m3); }
void VMSPublisher::putM2() { this->vmsSAL->putSample_m2(&this->m2ms); }
void VMSPublisher::putTMA() { this->vmsSAL->putSample_tma(&this->mtMount); }

} /* namespace VMS */
} /* namespace LSST */
