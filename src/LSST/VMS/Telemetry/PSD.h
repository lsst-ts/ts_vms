/*
 * PSD telemetry handling class.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef _VMS_Telemetry_PSD_
#define _VMS_Telemetry_PSD_

#include <complex>
#include <fftw3.h>

#include <SAL_MTVMS.h>

#include <VMSPublisher.h>

namespace LSST {
namespace VMS {
namespace Telemetry {

/**
 * PSD (Power Spectral Density) calculation.
 *
 * When enough datapoints are accumualted in cache, PSD is
 * cacluated and send over SAL/DDS.
 */
class PSD final : public MTVMS_psdC {
public:
    PSD(short int sensor);
    ~PSD(void);

    /**
     * Sets PSD configuration.
     *
     * @param dataPoints Number of points in PSD. According to Nyquist theorem,
     * at least dataPoints * 2 samples must be provided (with append call) to
     * compute PSD
     *
     * @param samplingFrequency sample frequency. Converts indices in output
     * FFT into frequencies.
     */
    void configure(size_t dataPoints, float samplingFrequency);

    /**
     * Append new data point.
     *
     * @param x measured X acceleration
     * @param y measured Y acceleration
     * @param z measured Z acceleration
     */
    void append(float x, float y, float z);

    /**
     * Calculate frequency from given index in the PSD array.
     *
     * @param index index of member in PSD
     *
     * @return frequency corresponding to the input index
     */
    float frequency(size_t index);

private:
    float _samplingPeriod;
    // axis cache
    size_t _cache_size;
    double* _cache[3];
    fftw_complex* _output[3];
    fftw_plan _plans[3];
};

}  // namespace Telemetry
}  // namespace VMS
}  // namespace LSST

#endif /** _VMS_Telemetry_PSD_ */
