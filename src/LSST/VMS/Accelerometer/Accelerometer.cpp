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
#include <VMSApplicationSettings.h>

#define AXES_PER_SENSOR 3
#define MAX_SAMPLE_PER_PUBLISH 50

namespace LSST {
namespace VMS {

Accelerometer::Accelerometer(FPGA *_fpga, VMSApplicationSettings *vmsApplicationSettings) {
    SPDLOG_DEBUG("Accelerometer::Accelerometer()");
    fpga = _fpga;
    m1m3Data = VMSPublisher::instance().getM1M3();
    m2Data = VMSPublisher::instance().getM2();
    cameraRotatorData = VMSPublisher::instance().getCameraRotator();
    // tmaData = VMSPublisher::instance().getTMA();

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
    SPDLOG_INFO("Accelerometer: enableAccelerometers()");
    uint16_t buffer[2] = {FPGAAddresses::Accelerometers, true};
    fpga->writeCommandFIFO(buffer, 2, 20);
}

void Accelerometer::disableAccelerometers() {
    SPDLOG_INFO("Accelerometer: disableAccelerometers()");
    uint16_t buffer[2] = {FPGAAddresses::Accelerometers, false};
    fpga->writeCommandFIFO(buffer, 2, 20);
}

void Accelerometer::sampleData() {
    SPDLOG_TRACE("Accelerometer: sampleData()");
    fpga->writeRequestFIFO(FPGAAddresses::Accelerometers, 0);
    fpga->readU64ResponseFIFO(u64Buffer, MAX_SAMPLE_PER_PUBLISH, 30);
    fpga->readSGLResponseFIFO(sglBuffer, numberOfSensors * AXES_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH, 500);
    switch (subsystem) {
        case M1M3:
            processM1M3();
            break;
        case M2:
            processM2();
            break;
        case CameraRotator:
            processCameraRotator();
            break;
        case TMA:
            processTMA();
            break;
    }
}

#define SAMPLES_TO_SAL_3(data)                                      \
    data->timestamp = Timestamp::fromRaw(u64Buffer[0]);             \
    size_t dataBufferIndex = 0;                                     \
    for (size_t i = 0; i < MAX_SAMPLE_PER_PUBLISH; ++i) {           \
        data->sensor1XAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor1YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor1ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor2XAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor2YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor2ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor3XAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor3YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor3ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
    }

#define SAMPLES_TO_SAL_6(data)                                      \
    data->timestamp = Timestamp::fromRaw(u64Buffer[0]);             \
    size_t dataBufferIndex = 0;                                     \
    for (size_t i = 0; i < MAX_SAMPLE_PER_PUBLISH; ++i) {           \
        data->sensor1XAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor1YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor1ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor2XAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor2YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor2ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor3XAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor3YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor3ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor4YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor4ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor5XAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor5YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor5ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor6XAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor6YAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
        data->sensor6ZAcceleration[i] = sglBuffer[dataBufferIndex]; \
        dataBufferIndex++;                                          \
    }

void Accelerometer::processM1M3() {
    SPDLOG_TRACE("Accelerometer: processM1M3()");
    SAMPLES_TO_SAL_3(m1m3Data);
    VMSPublisher::instance().putM1M3();
}

void Accelerometer::processM2() {
    SPDLOG_TRACE("Accelerometer: processM2()");
    SAMPLES_TO_SAL_6(m2Data);
    VMSPublisher::instance().putM2();
}

void Accelerometer::processCameraRotator() {
    SPDLOG_TRACE("Accelerometer: processCameraRotator()");
    SAMPLES_TO_SAL_3(cameraRotatorData);
    VMSPublisher::instance().putM2();
}

void Accelerometer::processTMA() {
    SPDLOG_ERROR("Accelerometer: processTMA()");
    // SAMPLES_TO_SAL_3(tmaData);
    // VMSPublisher::instance().putTMA();
}

} /* namespace VMS */
} /* namespace LSST */
