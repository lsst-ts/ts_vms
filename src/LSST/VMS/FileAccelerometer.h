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

#ifndef __FILE_ACCELEROMETER_H_
#define __FILE_ACCELEROMETER_H_

#include <filesystem>
#include <fstream>

#include <Accelerometer.h>

namespace LSST {
namespace VMS {

/**
 * VMS File Accelerometer sampling.
 */
class FileAccelerometer : public Accelerometer {
public:
    FileAccelerometer(VMSApplicationSettings *vmsApplicationSettings, std::filesystem::path file_path);
    virtual ~FileAccelerometer(void);

    void flush() { _ofile.flush(); }

protected:
    void processData(int sensor, float acc_x, float acc_y, float acc_z) override;

private:
    std::ofstream _ofile;
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* __FILE_ACCELEROMETER_H_ */
