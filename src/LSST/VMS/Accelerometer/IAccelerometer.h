/*
 * IAccelerometer.h
 *
 *  Created on: Nov 1, 2017
 *      Author: ccontaxis
 */

#ifndef IACCELEROMETER_H_
#define IACCELEROMETER_H_

namespace LSST {
namespace VMS {

/*!
 * Interface for interacting with accelerometers.
 */
class IAccelerometer {
public:
  virtual ~IAccelerometer();

  /*!
   * Enable collecting data from the accelerometers.
   */
  virtual void enableAccelerometers();
  /*!
   * Disable collecting data from the accelerometers.
   */
  virtual void disableAccelerometers();

  /*!
   * Sample the accelerometers.
   */
  virtual void sampleData();
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* IACCELEROMETER_H_ */
