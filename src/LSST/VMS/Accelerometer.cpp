/*
 * Accelerometer.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: ccontaxis
 */

#include <Accelerometer.h>
#include <FPGAAddresses.h>
#include <VMSPublisher.h>
#include <spdlog/spdlog.h>
#include <Timestamp.h>

#define AXIS_PER_SENSOR 3
#define MAX_SAMPLE_PER_PUBLISH 50
#define G2M_S_2(g) (g * 9.80665)

namespace LSST {
namespace VMS {

Accelerometer::Accelerometer(FPGA *_fpga, VMSApplicationSettings *vmsApplicationSettings) {
    SPDLOG_DEBUG("Accelerometer::Accelerometer()");
    fpga = _fpga;

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

void Accelerometer::enableAccelerometers(uint32_t period, int16_t outputType) {
    SPDLOG_INFO("Accelerometer: enableAccelerometers()");
    fpga->setPeriod(period);
    fpga->setOutputType(outputType);
    fpga->setOperate(true);
}

void Accelerometer::disableAccelerometers() {
    SPDLOG_INFO("Accelerometer: disableAccelerometers()");
    fpga->setOperate(false);
}

void Accelerometer::sampleData() {
    SPDLOG_TRACE("Accelerometer: sampleData()");
    uint32_t buffer[numberOfSensors * AXIS_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH];

    fpga->readResponseFIFO(buffer, numberOfSensors * AXIS_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH, 1000);

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
