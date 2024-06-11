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

#include <stdio.h>
#include <chrono>
#include <cstring>
#include <filesystem>

#include <spdlog/spdlog.h>

#include <FileAccelerometer.h>

using namespace LSST::VMS;

FileAccelerometer::FileAccelerometer(VMSApplicationSettings *vmsApplicationSettings,
                                     std::filesystem::path file_path)
        : Accelerometer(vmsApplicationSettings) {
    SPDLOG_DEBUG("FlleAccelerometer::FileAccelerometer({})", file_path.string());

    auto ppath = file_path.parent_path();

    if (!ppath.empty()) {
        std::filesystem::create_directories(ppath);
    }
    _ofile.open(file_path, std::ofstream::out | std::ofstream::binary | std::ofstream::trunc);
    _ofile.exceptions(std::ios::badbit | std::ios::failbit);

    // some magic string to identify a file
    _ofile.write("VMS", 3);
    uint8_t d = _vmsApplicationSettings->Subsystem.length() + 1;
    _ofile.write(reinterpret_cast<char *>(&d), 1);
    _ofile.write(_vmsApplicationSettings->Subsystem.c_str(), d - 1);
    d = _vmsApplicationSettings->sensors;
    _ofile.write(reinterpret_cast<char *>(&d), 1);
}

FileAccelerometer::~FileAccelerometer(void) { _ofile.close(); }

void FileAccelerometer::processData(int sensor, float acc_x, float acc_y, float acc_z) {
    SPDLOG_TRACE("FileAccelerometer: processData()");

    if (sensor == 0) {
        auto now = std::chrono::system_clock::now();
        auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
        _ofile.write(reinterpret_cast<char *>(&micros), sizeof(micros));
    }

    _ofile.write(reinterpret_cast<char *>(&acc_x), sizeof(acc_x));
    _ofile.write(reinterpret_cast<char *>(&acc_y), sizeof(acc_x));
    _ofile.write(reinterpret_cast<char *>(&acc_z), sizeof(acc_x));
}
