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

#include <FPGA.h>
#include <SALAccelerometer.h>
#include <VMSPublisher.h>

#define AXIS_PER_SENSOR 3
#define MAX_SAMPLE_PER_PUBLISH 50
#define G2M_S_2(g) (g * 9.80665)

using namespace LSST::VMS;

SALAccelerometer::SALAccelerometer(VMSApplicationSettings *vmsApplicationSettings)
        : Accelerometer(vmsApplicationSettings) {
    SPDLOG_DEBUG("Accelerometer::Accelerometer()");

    _sampleData = new MTVMS_dataC[_vmsApplicationSettings->sensors];
    _psds = new Telemetry::PSD[_vmsApplicationSettings->sensors];
    _dataIndex = 0;

    float _periodInS = _vmsApplicationSettings->period / 1000.0f;

    for (int s = 0; s < _vmsApplicationSettings->sensors; s++) {
        _sampleData[s].sensor = s + 1;
        _psds[s].configure(s + 1, 1.0f / (2.0f * _periodInS), _periodInS);
    }
}

SALAccelerometer::~SALAccelerometer(void) {
    delete[] _psds;
    delete[] _sampleData;
}

void SALAccelerometer::processData(int sensor, float acc_x, float acc_y, float acc_z) {
    SPDLOG_TRACE("SALAccelerometer: processData() count {}", _dataIndex);

    if (_dataIndex == 0) {
        double data_timestamp = VMSPublisher::instance().getTimestamp();

        for (int s = 0; s < _vmsApplicationSettings->sensors; s++) {
            _sampleData[s].timestamp = data_timestamp;
        }
    }

    _sampleData[sensor].accelerationX[_dataIndex] = acc_x;
    _sampleData[sensor].accelerationY[_dataIndex] = acc_y;
    _sampleData[sensor].accelerationZ[_dataIndex] = acc_z;

    _psds[sensor].append(acc_x, acc_y, acc_z);

    if (sensor == _vmsApplicationSettings->sensors - 1) {
        _dataIndex++;

        if (_dataIndex >= MAX_SAMPLE_PER_PUBLISH) {
            for (int s = 0; s < _vmsApplicationSettings->sensors; s++) {
                VMSPublisher::instance().putData(&(_sampleData[s]));
            }
            _dataIndex = 0;
        }
    }
}
