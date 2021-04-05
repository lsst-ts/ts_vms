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

    vmsSAL->salTelemetryPub((char *)"MTVMS_data");
}

double VMSPublisher::getTimestamp() { return vmsSAL->getCurrentTime(); }

} /* namespace VMS */
} /* namespace LSST */
