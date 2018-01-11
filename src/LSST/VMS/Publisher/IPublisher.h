/*
 * IPublisher.h
 *
 *  Created on: Sep 26, 2017
 *      Author: ccontaxis
 */

#ifndef IPUBLISHER_H_
#define IPUBLISHER_H_

struct vms_M1M3C;
struct vms_M2C;
struct vms_TMAC;

namespace LSST {
namespace VMS {

/*!
 * The interface for publishing data.
 */
class IPublisher {
public:
	virtual ~IPublisher();

	/*!
	 * Get the M1M3 data structure.
	 * @return M1M3 data structure
	 */
	virtual vms_M1M3C* getM1M3();
	/*!
	 * Get the M2 data structure.
	 * @return M2 data structure
	 */
	virtual vms_M2C* getM2();
	/*!
	 * Get the TMA data structure.
	 * @return TMA data structure
	 */
	virtual vms_TMAC* getTMA();

	/*!
	 * Get the current time.
	 * @return The current time in seconds
	 */
	virtual double getTimestamp();

	/*!
	 * Publish the M1M3 data.
	 */
	virtual void putM1M3();
	/*!
	 * Publish the M2 data.
	 */
	virtual void putM2();
	/*!
	 * Publish the TMA data.
	 */
	virtual void putTMA();
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* IPUBLISHER_H_ */
