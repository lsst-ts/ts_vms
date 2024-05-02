/*
 * VMSPublisher.h
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#ifndef VMSPUBLISHER_H_
#define VMSPUBLISHER_H_

#include <SAL_MTVMS.h>
#include <cRIO/Singleton.h>

#include <memory>

namespace LSST {
namespace VMS {

/**
 * Singleton for DDS/SAL publisher.
 */
class VMSPublisher : public cRIO::Singleton<VMSPublisher> {
public:
    VMSPublisher(token);
    ~VMSPublisher();

    void setSAL(std::shared_ptr<SAL_MTVMS> sal);

    static std::shared_ptr<SAL_MTVMS> SAL() { return instance()._vmsSAL; }

    void setLogLevel(int newLevel);

    void logHeartbeat(MTVMS_logevent_heartbeatC *data) { _vmsSAL->logEvent_heartbeat(data, 0); }
    void logSoftwareVersions();
    void logSimulationMode();

    double getTimestamp();

    void putData(MTVMS_dataC *data) { _vmsSAL->putSample_data(data); }
    void putPsd(MTVMS_psdC *psd) { _vmsSAL->putSample_psd(psd); }

    void putMiscellaneous(float chassisTemperature, int64_t ticks) {
        _miscellaneous.chassisTemperature = chassisTemperature;
        _miscellaneous.ticks = ticks;
        _vmsSAL->putSample_miscellaneous(&_miscellaneous);
    }

private:
    std::shared_ptr<SAL_MTVMS> _vmsSAL;

    MTVMS_miscellaneousC _miscellaneous;

    MTVMS_logevent_logLevelC _logLevel;
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* VMSPUBLISHER_H_ */
