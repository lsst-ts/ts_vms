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
    _psds = new Telemetry::PSD[_numberOfSensors];
    _dataIndex = 0;

    float _periodInS = _vmsApplicationSettings->period / 1000.0f;

    for (int s = 0; s < _numberOfSensors; s++) {
        _sampleData[s].sensor = s + 1;
        _psds[s].configure(s + 1, 1.0f / (2.0f * _periodInS), _periodInS);
    }
}

Accelerometer::~Accelerometer(void) {
    delete[] _psds;
    delete[] _sampleData;
}

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
        float acc_x = _convert(&dataBuffer);
        float acc_y = _convert(&dataBuffer);
        float acc_z = _convert(&dataBuffer);

        _sampleData[s].accelerationX[_dataIndex] = acc_x;
        _sampleData[s].accelerationY[_dataIndex] = acc_y;
        _sampleData[s].accelerationZ[_dataIndex] = acc_z;

        _psds[s].append(acc_x, acc_y, acc_z);
    }

    _dataIndex++;

    if (_dataIndex >= MAX_SAMPLE_PER_PUBLISH) {
        for (int s = 0; s < _numberOfSensors; s++) {
            VMSPublisher::instance().putData(&(_sampleData[s]));
        }
        _dataIndex = 0;
    }
}

float Accelerometer::_convert(uint32_t **data) {
    union {
        uint32_t i;
        float f;
    } u32float;
    switch (_vmsApplicationSettings->outputType) {
        case 1:
        case 2:
            u32float.f = G2M_S_2(NiFpga_ConvertFromFxpToFloat(ResponseFxpTypeInfo, **data));
            ++*data;
            return u32float.f;
        case 3:
            u32float.i = **data;
            ++*data;
            return u32float.f;
        case 50:
            *data += 3;
            u32float.i = **data;
            ++*data;
            return u32float.f;
        default:
            return NAN;
    }
}

} /* namespace VMS */
} /* namespace LSST */
