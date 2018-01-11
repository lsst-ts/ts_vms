/*
 * IPublisher.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#include <IPublisher.h>

namespace LSST {
namespace VMS {

IPublisher::~IPublisher() { }

vms_M1M3C* IPublisher::getM1M3() { return 0; }
vms_M2C* IPublisher::getM2() { return 0; }
vms_TMAC* IPublisher::getTMA() { return 0; }

double IPublisher::getTimestamp() { return 0; }

void IPublisher::putM1M3() { }
void IPublisher::putM2() { }
void IPublisher::putTMA() { }

} /* namespace VMS */
} /* namespace LSST */
