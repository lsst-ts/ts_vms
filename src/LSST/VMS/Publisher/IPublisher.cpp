/*
 * IPublisher.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#include <IPublisher.h>

namespace LSST {
namespace VMS {

IPublisher::~IPublisher() {}

MTVMS_m1m3C *IPublisher::getM1M3() { return 0; }
MTVMS_m2C *IPublisher::getM2() { return 0; }
MTVMS_tmaC *IPublisher::getTMA() { return 0; }

double IPublisher::getTimestamp() { return 0; }

void IPublisher::putM1M3() {}
void IPublisher::putM2() {}
void IPublisher::putTMA() {}

} /* namespace VMS */
} /* namespace LSST */
