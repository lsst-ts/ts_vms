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

#include <spdlog/spdlog.h>

#include <Accelerometer.h>
#include <FPGA.h>

#define G2M_S_2(g) (g * 9.80665)

using namespace LSST::VMS;

Accelerometer::Accelerometer(VMSApplicationSettings *vmsApplicationSettings) {
    SPDLOG_DEBUG("Accelerometer::Accelerometer()");

    _vmsApplicationSettings = vmsApplicationSettings;
}

Accelerometer::~Accelerometer(void) {}

void Accelerometer::enableAccelerometers() {
    SPDLOG_INFO("Accelerometer: enableAccelerometers(), period {}, output type {}",
                _vmsApplicationSettings->period, _vmsApplicationSettings->outputType);
    FPGA::instance().setPeriodOutputType(_vmsApplicationSettings->period,
                                         _vmsApplicationSettings->outputType);
    FPGA::instance().setOperate(true);
}

void Accelerometer::disableAccelerometers() {
    SPDLOG_INFO("Accelerometer: disableAccelerometers()");
    FPGA::instance().setOperate(false);
}

void Accelerometer::sampleData() {
    SPDLOG_TRACE("Accelerometer: sampleData()");
    float average[_vmsApplicationSettings->sensors * AXIS_PER_SENSOR];
    float max[_vmsApplicationSettings->sensors * AXIS_PER_SENSOR];
    float min[_vmsApplicationSettings->sensors * AXIS_PER_SENSOR];

    FPGA::instance().readResponseFIFOs(min, max, average, _vmsApplicationSettings->sensors * AXIS_PER_SENSOR,
                                       _vmsApplicationSettings->period * 2);

    float *dataBuffer = average;
    for (int s = 0; s < _vmsApplicationSettings->sensors; s++) {
        float acc_x = *dataBuffer;
        dataBuffer++;
        float acc_y = *dataBuffer;
        dataBuffer++;
        float acc_z = *dataBuffer;
        dataBuffer++;

        processData(s, acc_x, acc_y, acc_z);
    }
}

void Accelerometer::rawData(int length) {
    SPDLOG_TRACE("Accelerometer: sampleData()");

    float raw[_vmsApplicationSettings->sensors * AXIS_PER_SENSOR * length];

    FPGA::instance().readRawFIFO(raw, _vmsApplicationSettings->sensors * AXIS_PER_SENSOR * length,
                                 _vmsApplicationSettings->period);

    RawData records[_vmsApplicationSettings->sensors];

    float *dataBuffer = raw;
    for (int i = 0; i < length; i++) {
        for (int s = 0; s < _vmsApplicationSettings->sensors; s++) {
            float acc_x = *dataBuffer;
            dataBuffer++;
            float acc_y = *dataBuffer;
            dataBuffer++;
            float acc_z = *dataBuffer;
            dataBuffer++;

            records[s].push_back(RawRecord(acc_x, acc_y, acc_z));
        }
    }

    for (int s = 0; s < _vmsApplicationSettings->sensors; s++) {
        processRawData(s, records[s]);
    }
}
