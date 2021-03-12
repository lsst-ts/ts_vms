/*
 * VMSPublisher.h
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#ifndef VMSPUBLISHER_H_
#define VMSPUBLISHER_H_

#include <IPublisher.h>
#include <SAL_MTVMSC.h>

#include <memory>

class SAL_MTVMS;

namespace LSST {
namespace VMS {

class VMSPublisher : public IPublisher {
private:
  std::shared_ptr<SAL_MTVMS> vmsSAL;

  MTVMS_m1m3C m1m3;
  MTVMS_m2C m2ms;
  MTVMS_tmaC mtMount;

public:
  VMSPublisher(std::shared_ptr<SAL_MTVMS> vmsSAL);

  MTVMS_m1m3C *getM1M3() { return &this->m1m3; }
  MTVMS_m2C *getM2() { return &this->m2ms; }
  MTVMS_tmaC *getTMA() { return &this->mtMount; }

  double getTimestamp();

  void putM1M3();
  void putM2();
  void putTMA();
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* VMSPUBLISHER_H_ */
