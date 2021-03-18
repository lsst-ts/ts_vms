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
#include <SAL_MTVMSC.h>
#include <Timestamp.h>
#include <VMSApplicationSettings.h>

#define AXES_PER_SENSOR 3
#define MAX_SAMPLE_PER_PUBLISH 50

namespace LSST {
namespace VMS {

Accelerometer::Accelerometer(FPGA *_fpga, VMSApplicationSettings *_vmsApplicationSettings) {
    SPDLOG_DEBUG("Accelerometer::Accelerometer()");
    fpga = _fpga;
    vmsApplicationSettings = _vmsApplicationSettings;
    m1m3Data = VMSPublisher::instance().getM1M3();
    m2Data = VMSPublisher::instance().getM2();
    tmaData = VMSPublisher::instance().getTMA();
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
    fpga->readU64ResponseFIFO(u64Buffer, MAX_SAMPLE_PER_PUBLISH, 15);
    fpga->readSGLResponseFIFO(
            sglBuffer, vmsApplicationSettings->NumberOfSensors * AXES_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH,
            100);
    if (vmsApplicationSettings->Subsystem == "M1M3") {
        processM1M3();
    } else if (vmsApplicationSettings->Subsystem == "M2") {
        processM2();
    } else if (vmsApplicationSettings->Subsystem == "TMA") {
        processTMA();
    }
}

void Accelerometer::processM1M3() {
    SPDLOG_TRACE("Accelerometer: processM1M3()");
    m1m3Data->timestamp = Timestamp::fromRaw(u64Buffer[0]);
    int32_t dataBufferIndex = 0;
    for (int i = 0; i < MAX_SAMPLE_PER_PUBLISH; ++i) {
        m1m3Data->sensor1XAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        m1m3Data->sensor1YAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        m1m3Data->sensor1ZAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        m1m3Data->sensor2XAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        m1m3Data->sensor2YAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        m1m3Data->sensor2ZAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        m1m3Data->sensor3XAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        m1m3Data->sensor3YAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        m1m3Data->sensor3ZAcceleration[i] = sglBuffer[dataBufferIndex];
        dataBufferIndex++;
    }
    VMSPublisher::instance().putM1M3();
}

void Accelerometer::processM2() {
    SPDLOG_INFO("Accelerometer: processM2()");
    m2Data->timestamp = Timestamp::fromRaw(u64Buffer[0]);
    // TODO: Populate M2
    VMSPublisher::instance().putM2();
}

void Accelerometer::processTMA() {
    SPDLOG_INFO("Accelerometer: processTMA()");
    tmaData->timestamp = Timestamp::fromRaw(u64Buffer[0]);
    // TODO: Populate TMA
    VMSPublisher::instance().putTMA();
}

} /* namespace VMS */
} /* namespace LSST */
