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

    double getTimestamp();

    void putData(MTVMS_dataC *data) { vmsSAL->putSample_data(data); }

private:
    std::shared_ptr<SAL_MTVMS> vmsSAL;
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* VMSPUBLISHER_H_ */
