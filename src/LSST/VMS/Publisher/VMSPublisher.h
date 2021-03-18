/*
 * VMSPublisher.h
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#ifndef VMSPUBLISHER_H_
#define VMSPUBLISHER_H_

#include <SAL_MTVMSC.h>
#include <cRIO/Singleton.h>

#include <memory>

class SAL_MTVMS;

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
    MTVMS_m2C *getM2() { return &m2ms; }
    MTVMS_tmaC *getTMA() { return &mtMount; }

    double getTimestamp();

    void putM1M3();
    void putM2();
    void putTMA();

private:
    std::shared_ptr<SAL_MTVMS> vmsSAL;

    MTVMS_m1m3C m1m3;
    MTVMS_m2C m2ms;
    MTVMS_tmaC mtMount;
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* VMSPUBLISHER_H_ */
