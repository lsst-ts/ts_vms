/*
 * VMSPublisher.cpp
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#include <spdlog/spdlog.h>
#include <SAL_MTVMS.h>
#include <VMSPublisher.h>

namespace LSST {
namespace VMS {

VMSPublisher::VMSPublisher(token) {
    SPDLOG_DEBUG("VMSPublisher: VMSPublisher()");
    vmsSAL = NULL;
}

VMSPublisher::~VMSPublisher() {}

void VMSPublisher::setSAL(std::shared_ptr<SAL_MTVMS> sal) {
    SPDLOG_DEBUG("VMSPublisher::setSAL()");
    vmsSAL = sal;

    vmsSAL->salTelemetryPub((char *)"MTVMS_m1m3");
    vmsSAL->salTelemetryPub((char *)"MTVMS_m2");
    vmsSAL->salTelemetryPub((char *)"MTVMS_tma");
}

double VMSPublisher::getTimestamp() { return vmsSAL->getCurrentTime(); }

void VMSPublisher::putM1M3() { vmsSAL->putSample_m1m3(&m1m3); }
void VMSPublisher::putM2() { vmsSAL->putSample_m2(&m2ms); }
void VMSPublisher::putTMA() { vmsSAL->putSample_tma(&mtMount); }

} /* namespace VMS */
} /* namespace LSST */
