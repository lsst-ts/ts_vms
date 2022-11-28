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

#include <Accelerometer.h>
#include <FPGA.h>
#include <FPGAAddresses.h>
#include <VMSPublisher.h>
#include <spdlog/spdlog.h>
#include <Timestamp.h>

#define AXIS_PER_SENSOR 3
#define MAX_SAMPLE_PER_PUBLISH 50
#define G2M_S_2(g) (g * 9.80665)

namespace LSST {
namespace VMS {

Accelerometer::Accelerometer(VMSApplicationSettings *vmsApplicationSettings) {
    SPDLOG_DEBUG("Accelerometer::Accelerometer()");

    _vmsApplicationSettings = vmsApplicationSettings;

    if (vmsApplicationSettings->Subsystem == "M1M3") {
        subsystem = M1M3;
        numberOfSensors = 3;
    } else if (vmsApplicationSettings->Subsystem == "M2") {
        subsystem = M2;
        numberOfSensors = 6;
    } else if (vmsApplicationSettings->Subsystem == "CameraRotator") {
        subsystem = CameraRotator;
        numberOfSensors = 3;
    } else if (vmsApplicationSettings->Subsystem == "TMA") {
        subsystem = TMA;
        numberOfSensors = 3;
    } else {
        SPDLOG_ERROR("Unknown subsystem: {}", vmsApplicationSettings->Subsystem);
        exit(EXIT_FAILURE);
    }
}

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
    SPDLOG_TRACE("Accelerometer: sampleData()");
    uint32_t buffer[numberOfSensors * AXIS_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH];

    FPGA::instance().readResponseFIFO(buffer, numberOfSensors * AXIS_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH,
                                      1000);

    MTVMS_dataC data[numberOfSensors];

    double data_timestamp = VMSPublisher::instance().getTimestamp();

    for (int s = 0; s < numberOfSensors; s++) {
        data[s].timestamp = data_timestamp;
        data[s].sensor = s + 1;
    }

    uint32_t *dataBuffer = buffer;
    for (int i = 0; i < MAX_SAMPLE_PER_PUBLISH; i++) {
        for (int s = 0; s < numberOfSensors; s++) {
            data[s].accelerationX[i] =
                    G2M_S_2(NiFpga_ConvertFromFxpToFloat(ResponseFxpTypeInfo, *dataBuffer));
            dataBuffer++;
            data[s].accelerationY[i] =
                    G2M_S_2(NiFpga_ConvertFromFxpToFloat(ResponseFxpTypeInfo, *dataBuffer));
            dataBuffer++;
            data[s].accelerationZ[i] =
                    G2M_S_2(NiFpga_ConvertFromFxpToFloat(ResponseFxpTypeInfo, *dataBuffer));
            dataBuffer++;
        }
    }

    for (int s = 0; s < numberOfSensors; s++) {
        VMSPublisher::instance().putData(&(data[s]));
    }
}

} /* namespace VMS */
} /* namespace LSST */
