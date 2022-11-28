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
    _vmsSAL = NULL;
}

VMSPublisher::~VMSPublisher() {}

void VMSPublisher::setSAL(std::shared_ptr<SAL_MTVMS> sal) {
    SPDLOG_DEBUG("VMSPublisher::setSAL()");
    _vmsSAL = sal;

    SPDLOG_DEBUG("VMSPublisher: Initializing SAL Telemetry");
    _vmsSAL->salTelemetryPub((char *)"MTVMS_data");
    _vmsSAL->salTelemetryPub((char *)"MTVMS_miscellaneous");

    SPDLOG_DEBUG("VMSPublisher: Initializing SAL Events");
    _vmsSAL->salEventPub((char *)"MTVMS_logevent_heartbeat");
    _vmsSAL->salEventPub((char *)"MTVMS_logevent_logLevel");
    _vmsSAL->salEventPub((char *)"MTVMS_logevent_summaryState");
    _vmsSAL->salEventPub((char *)"MTVMS_logevent_softwareVersions");
    _vmsSAL->salEventPub((char *)"MTVMS_logevent_simulationMode");
    _vmsSAL->salEventPub((char *)"MTVMS_logevent_fpgaState");
}

void VMSPublisher::setLogLevel(int newLevel) {
    if (_logLevel.level != newLevel) {
        SPDLOG_TRACE("logEvent_logLevel {}", newLevel);
        _logLevel.level = newLevel;
        _vmsSAL->logEvent_logLevel(&_logLevel, 0);
    }
}

void VMSPublisher::logSoftwareVersions() {
    MTVMS_logevent_softwareVersionsC versions;
    versions.salVersion = SAL_MTVMS::getSALVersion();
    versions.xmlVersion = SAL_MTVMS::getXMLVersion();
    versions.openSpliceVersion = SAL_MTVMS::getOSPLVersion();
    versions.cscVersion = VERSION;
    versions.subsystemVersions = "";
    _vmsSAL->logEvent_softwareVersions(&versions, 0);
}

void VMSPublisher::logSimulationMode() {
    MTVMS_logevent_simulationModeC simulation;
#ifdef SIMULATOR
    simulation.mode = 1;
#else
    simulation.mode = 0;
#endif
    _vmsSAL->logEvent_simulationMode(&simulation, 0);
}

double VMSPublisher::getTimestamp() { return _vmsSAL->getCurrentTime(); }

} /* namespace VMS */
} /* namespace LSST */
