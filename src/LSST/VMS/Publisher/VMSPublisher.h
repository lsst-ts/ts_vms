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

    MTVMS_m1m3C *getM1M3() { return &m1m3; }
    MTVMS_m2C *getM2() { return &m2; }
    MTVMS_cameraRotatorC *getCameraRotator() { return &cameraRotator; }
    // MTVMS_tmaC *getTMA() { return &mtMount; }

    double getTimestamp();

    void putM1M3() { vmsSAL->putSample_m1m3(&m1m3); }
    void putM2() { vmsSAL->putSample_m2(&m2); }
    void putCameraRotator() { vmsSAL->putSample_cameraRotator(&cameraRotator); }
    // void putTMA() { vmsSAL->putSample_tma(&mtMount); }

private:
    std::shared_ptr<SAL_MTVMS> vmsSAL;

    MTVMS_m1m3C m1m3;
    MTVMS_m2C m2;
    MTVMS_cameraRotatorC cameraRotator;
    // MTVMS_tmaC mtMount;
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* VMSPUBLISHER_H_ */
