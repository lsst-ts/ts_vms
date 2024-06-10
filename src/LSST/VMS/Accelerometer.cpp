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
    uint32_t buffer[_vmsApplicationSettings->sensors * AXIS_PER_SENSOR];

    FPGA::instance().readResponseFIFO(buffer, _vmsApplicationSettings->sensors * AXIS_PER_SENSOR,
                                      _vmsApplicationSettings->period * 2);

    uint32_t *dataBuffer = buffer;
    for (int s = 0; s < _vmsApplicationSettings->sensors; s++) {
        float acc_x = _convert(&dataBuffer);
        float acc_y = _convert(&dataBuffer);
        float acc_z = _convert(&dataBuffer);

        processData(s, acc_x, acc_y, acc_z);
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
