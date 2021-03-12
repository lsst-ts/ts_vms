/*
 * Accelerometer.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: ccontaxis
 */

#include <Accelerometer.h>
#include <FPGAAddresses.h>
#include <IFPGA.h>
#include <IPublisher.h>
#include <Log.h>
#include <SAL_MTVMSC.h>
#include <Timestamp.h>
#include <VMSApplicationSettings.h>

#define AXES_PER_SENSOR 3
#define MAX_SAMPLE_PER_PUBLISH 50

namespace LSST {
namespace VMS {

Accelerometer::Accelerometer(IPublisher *publisher, IFPGA *fpga,
                             VMSApplicationSettings *vmsApplicationSettings) {
    Log.Debug("Accelerometer::Accelerometer()");
    this->publisher = publisher;
    this->fpga = fpga;
    this->vmsApplicationSettings = vmsApplicationSettings;
    this->m1m3Data = this->publisher->getM1M3();
    this->m2Data = this->publisher->getM2();
    this->tmaData = this->publisher->getTMA();
}

void Accelerometer::enableAccelerometers() {
    Log.Info("Accelerometer: enableAccelerometers()");
    uint16_t buffer[2] = {FPGAAddresses::Accelerometers, true};
    this->fpga->writeCommandFIFO(buffer, 2, 20);
}

void Accelerometer::disableAccelerometers() {
    Log.Info("Accelerometer: disableAccelerometers()");
    uint16_t buffer[2] = {FPGAAddresses::Accelerometers, false};
    this->fpga->writeCommandFIFO(buffer, 2, 20);
}

void Accelerometer::sampleData() {
    Log.Trace("Accelerometer: sampleData()");
    this->fpga->writeRequestFIFO(FPGAAddresses::Accelerometers, 0);
    this->fpga->readU64ResponseFIFO(this->u64Buffer, MAX_SAMPLE_PER_PUBLISH, 15);
    this->fpga->readSGLResponseFIFO(
            this->sglBuffer,
            this->vmsApplicationSettings->NumberOfSensors * AXES_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH, 100);
    if (this->vmsApplicationSettings->Subsystem == "M1M3") {
        this->processM1M3();
    } else if (this->vmsApplicationSettings->Subsystem == "M2") {
        this->processM2();
    } else if (this->vmsApplicationSettings->Subsystem == "TMA") {
        this->processTMA();
    }
}

void Accelerometer::processM1M3() {
    Log.Trace("Accelerometer: processM1M3()");
    this->m1m3Data->timestamp = Timestamp::fromRaw(this->u64Buffer[0]);
    int32_t dataBufferIndex = 0;
    for (int i = 0; i < MAX_SAMPLE_PER_PUBLISH; ++i) {
        this->m1m3Data->sensor1XAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        this->m1m3Data->sensor1YAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        this->m1m3Data->sensor1ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        this->m1m3Data->sensor2XAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        this->m1m3Data->sensor2YAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        this->m1m3Data->sensor2ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        this->m1m3Data->sensor3XAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        this->m1m3Data->sensor3YAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
        this->m1m3Data->sensor3ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
        dataBufferIndex++;
    }
    this->publisher->putM1M3();
}

void Accelerometer::processM2() {
    Log.Info("Accelerometer: processM2()");
    this->m2Data->timestamp = Timestamp::fromRaw(this->u64Buffer[0]);
    // TODO: Populate M2
    this->publisher->putM2();
}

void Accelerometer::processTMA() {
    Log.Info("Accelerometer: processTMA()");
    this->tmaData->timestamp = Timestamp::fromRaw(this->u64Buffer[0]);
    // TODO: Populate TMA
    this->publisher->putTMA();
}

} /* namespace VMS */
} /* namespace LSST */
