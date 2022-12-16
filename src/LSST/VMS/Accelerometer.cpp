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
#include <FPGAAddresses.h>
#include <VMSPublisher.h>

#define AXIS_PER_SENSOR 3
#define MAX_SAMPLE_PER_PUBLISH 50
#define G2M_S_2(g) (g * 9.80665)

namespace LSST {
namespace VMS {

Accelerometer::Accelerometer(VMSApplicationSettings *vmsApplicationSettings) {
    SPDLOG_DEBUG("Accelerometer::Accelerometer()");

    _vmsApplicationSettings = vmsApplicationSettings;

    if (vmsApplicationSettings->Subsystem == "M1M3") {
        _subsystem = M1M3;
        _numberOfSensors = 3;
    } else if (vmsApplicationSettings->Subsystem == "M2") {
        _subsystem = M2;
        _numberOfSensors = 6;
    } else if (vmsApplicationSettings->Subsystem == "CameraRotator") {
        _subsystem = CameraRotator;
        _numberOfSensors = 3;
    } else if (vmsApplicationSettings->Subsystem == "TMA") {
        _subsystem = TMA;
        _numberOfSensors = 3;
    } else {
        SPDLOG_ERROR("Unknown subsystem: {}", vmsApplicationSettings->Subsystem);
        exit(EXIT_FAILURE);
    }
    _sampleData = new MTVMS_dataC[_numberOfSensors];
    _dataIndex = 0;

    for (int s = 0; s < _numberOfSensors; s++) {
        _sampleData[s].sensor = s + 1;
    }
}

Accelerometer::~Accelerometer(void) { delete[] _sampleData; }

void Accelerometer::enableAccelerometers() {
    SPDLOG_INFO("Accelerometer: enableAccelerometers(), period {}, output type {}",
                _vmsApplicationSettings->period, _vmsApplicationSettings->outputType);
    FPGA::instance().setPeriod(_vmsApplicationSettings->period);
    FPGA::instance().setOutputType(_vmsApplicationSettings->outputType);
    FPGA::instance().setOperate(true);
}

void Accelerometer::disableAccelerometers() {
    SPDLOG_INFO("Accelerometer: disableAccelerometers()");
    FPGA::instance().setOperate(false);
}

void Accelerometer::sampleData() {
    SPDLOG_TRACE("Accelerometer: sampleData() count {}", _dataIndex);
    uint32_t buffer[_numberOfSensors * AXIS_PER_SENSOR];

    FPGA::instance().readResponseFIFO(buffer, _numberOfSensors * AXIS_PER_SENSOR,
                                      _vmsApplicationSettings->period * 2);

    if (_dataIndex == 0) {
        double data_timestamp = VMSPublisher::instance().getTimestamp();

        for (int s = 0; s < _numberOfSensors; s++) {
            _sampleData[s].timestamp = data_timestamp;
        }
    }

    uint32_t *dataBuffer = buffer;
    for (int s = 0; s < _numberOfSensors; s++) {
        _sampleData[s].accelerationX[_dataIndex] =
                G2M_S_2(NiFpga_ConvertFromFxpToFloat(ResponseFxpTypeInfo, *dataBuffer));
        dataBuffer++;
        _sampleData[s].accelerationY[_dataIndex] =
                G2M_S_2(NiFpga_ConvertFromFxpToFloat(ResponseFxpTypeInfo, *dataBuffer));
        dataBuffer++;
        _sampleData[s].accelerationZ[_dataIndex] =
                G2M_S_2(NiFpga_ConvertFromFxpToFloat(ResponseFxpTypeInfo, *dataBuffer));
        dataBuffer++;
    }

    _dataIndex++;

    if (_dataIndex >= MAX_SAMPLE_PER_PUBLISH) {
        for (int s = 0; s < _numberOfSensors; s++) {
            VMSPublisher::instance().putData(&(_sampleData[s]));
        }
        _dataIndex = 0;
    }
}

} /* namespace VMS */
} /* namespace LSST */
