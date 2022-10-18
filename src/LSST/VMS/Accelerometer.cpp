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

    _firstSample = true;
}

void Accelerometer::enableAccelerometers() {
    SPDLOG_INFO("Accelerometer: enableAccelerometers()");
    uint16_t buffer[2] = {FPGAAddresses::Accelerometers, true};
    fpga->writeCommandFIFO(buffer, 2, 20);
    _firstSample = true;
}

void Accelerometer::disableAccelerometers() {
    SPDLOG_INFO("Accelerometer: disableAccelerometers()");
    uint16_t buffer[2] = {FPGAAddresses::Accelerometers, false};
    fpga->writeCommandFIFO(buffer, 2, 20);
}

void Accelerometer::sampleData() {
    SPDLOG_TRACE("Accelerometer: sampleData()");
    uint64_t u64Buffer[MAX_SAMPLE_PER_PUBLISH];
    float sglBuffer[numberOfSensors * AXIS_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH];

    fpga->writeRequestFIFO(FPGAAddresses::Accelerometers, 0);
    fpga->readU64ResponseFIFO(u64Buffer, MAX_SAMPLE_PER_PUBLISH, _firstSample ? 500 : 70);
    fpga->readSGLResponseFIFO(sglBuffer, numberOfSensors * AXIS_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH, 10);
    _firstSample = false;

    double data_timestamp = Timestamp::fromRaw(u64Buffer[0]);

    MTVMS_dataC data[numberOfSensors];

    for (int s = 0; s < numberOfSensors; s++) {
        data[s].timestamp = data_timestamp;
        data[s].sensor = s + 1;
    }

    float *dataBuffer = sglBuffer;

    for (int i = 0; i < MAX_SAMPLE_PER_PUBLISH; i++) {
        for (int s = 0; s < numberOfSensors; s++) {
            data[s].accelerationX[i] = G2M_S_2(*dataBuffer);
            dataBuffer++;
            data[s].accelerationY[i] = G2M_S_2(*dataBuffer);
            dataBuffer++;
            data[s].accelerationZ[i] = G2M_S_2(*dataBuffer);
            dataBuffer++;
        }
    }

    for (int s = 0; s < numberOfSensors; s++) {
        VMSPublisher::instance().putData(&(data[s]));
    }
}

} /* namespace VMS */
} /* namespace LSST */