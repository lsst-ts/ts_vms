/*
 * IFPGA.h
 *
 *  Created on: Oct 2, 2017
 *      Author: ccontaxis
 */

#ifndef IFPGA_H_
#define IFPGA_H_

#include <NiFpga.h>

namespace LSST {
namespace VMS {

/*!
 * An interface for accessing FPGA functionality.
 */
class IFPGA {
public:
  virtual ~IFPGA();

  /*!
   * Initializes the FPGA library.
   * @return Status code.
   */
  virtual int32_t initialize();
  /*!
   * Opens the connection to the FPGA.
   * @return Status code.
   */
  virtual int32_t open();
  /*!
   * Closes the connection to the FPGA.
   * @return Status code.
   */
  virtual int32_t close();
  /*!
   * Finalizes the FPGA library.
   * @return Status code.
   */
  virtual int32_t finalize();

  /*!
   * Returns true if the provided status is an error code.
   * @return True if the status is an error code.
   */
  virtual bool isErrorCode(int32_t status);

  /*!
   * Sets the timestamp of the FPGA.
   * @param[in] timestamp The timestamp.
   * @return Status code.
   */
  virtual int32_t setTimestamp(double timestamp);

  /*!
   * Waits for the outer loop clock IRQ to trigger or for the
   * timeout to occur.
   * @param[in] timeout The timeout in ms. -1 is infinite.
   * @return Status code.
   */
  virtual int32_t waitForOuterLoopClock(int32_t timeout);
  /*!
   * Acknowledges the outer loop clock IRQ allowing the clock
   * to continue.
   * @return Status code.
   */
  virtual int32_t ackOuterLoopClock();

  /*!
   * Writes data to the command FIFO.
   * @param[in] data The buffer to write to the FIFO.
   * @param[in] length The number of elements in the buffer to write.
   * @param[in] timeoutInMs The timeout in ms. -1 is infinite.
   * @return Status code.
   */
  virtual int32_t writeCommandFIFO(uint16_t *data, int32_t length,
                                   int32_t timeoutInMs);

  /*!
   * Writes data to the request FIFO.
   * @param[in] data The buffer to write to the FIFO.
   * @param[in] length The number of elements in the buffer to write.
   * @param[in] timeoutInMs The timeout in ms. -1 is infinite.
   * @return Status code.
   */
  virtual int32_t writeRequestFIFO(uint16_t *data, int32_t length,
                                   int32_t timeoutInMs);

  /*!
   * Writes data to the request FIFO.
   * @param[in] data The buffer to write to the FIFO.
   * @param[in] timeoutInMs The timeout in ms. -1 is infinite.
   * @return Status code.
   */
  virtual int32_t writeRequestFIFO(uint16_t data, int32_t timeoutInMs);

  /*!
   * Reads data from the U16 response FIFO.
   * @param[in] data The buffer to write to the FIFO.
   * @param[in] length The number of elements in the buffer to write.
   * @param[in] timeoutInMs The timeout in ms. -1 is infinite.
   * @return Status code.
   */
  virtual int32_t readU64ResponseFIFO(uint64_t *data, int32_t length,
                                      int32_t timeoutInMs);

  /*!
   * Reads data from the SGL response FIFO.
   * @param[in] data The buffer to write to the FIFO.
   * @param[in] length The number of elements in the buffer to write.
   * @param[in] timeoutInMs The timeout in ms. -1 is infinite.
   * @return Status code.
   */
  virtual int32_t readSGLResponseFIFO(float *data, int32_t length,
                                      int32_t timeoutInMs);
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* IFPGA_H_ */
