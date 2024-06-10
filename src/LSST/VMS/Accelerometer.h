/*
 * This file is part of LSST MT VMS package.
 *
 * Developed for the Vera C. Rubin Telescope and Site System.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef __ACCELEROMETER_H_
#define __ACCELEROMETER_H_

#include <VMSApplicationSettings.h>

namespace LSST {
namespace VMS {

constexpr static int AXIS_PER_SENSOR = 3;

/**
 * VMS Accelerometer sampling.
 */
class Accelerometer {
public:
    Accelerometer(VMSApplicationSettings *vmsApplicationSettings);
    virtual ~Accelerometer(void);

    void enableAccelerometers();
    void disableAccelerometers();

    void sampleData();

protected:
    VMSApplicationSettings *_vmsApplicationSettings;

    virtual void processData(int sensor, float acc_x, float acc_y, float acc_z) = 0;

private:
    enum { M1M3, M2, CameraRotator, TMA } _subsystem;

    float _convert(uint32_t **data);
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* __ACCELEROMETER_H_ */
