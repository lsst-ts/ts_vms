/*
 * Accelerometer.cpp
 *
 *  Created on: Nov 1, 2017
 *      Author: ccontaxis
 */

#include <Accelerometer.h>
#include <IPublisher.h>
#include <IFPGA.h>
#include <FPGAAddresses.h>
#include <VMSApplicationSettings.h>
#include <Timestamp.h>
#include <SAL_vmsC.h>

#define AXES_PER_SENSOR 3
#define MAX_SAMPLE_PER_PUBLISH 50

namespace LSST {
namespace VMS {

Accelerometer::Accelerometer(IPublisher* publisher, IFPGA* fpga, VMSApplicationSettings* vmsApplicationSettings) {
	this->publisher = publisher;
	this->fpga = fpga;
	this->vmsApplicationSettings = vmsApplicationSettings;
	this->m1m3Data = this->publisher->getM1M3();
	this->m2Data = this->publisher->getM2();
	this->tmaData = this->publisher->getTMA();
}

void Accelerometer::enableAccelerometers() {
	uint16_t buffer[2] = { FPGAAddresses::Accelerometers, true };
	this->fpga->writeCommandFIFO(buffer, 2, 20);
}

void Accelerometer::disableAccelerometers() {
	uint16_t buffer[2] = { FPGAAddresses::Accelerometers, false };
	this->fpga->writeCommandFIFO(buffer, 2, 20);
}

void Accelerometer::sampleData() {
	this->fpga->writeRequestFIFO(FPGAAddresses::Accelerometers, 0);
	this->fpga->readU64ResponseFIFO(this->u64Buffer, MAX_SAMPLE_PER_PUBLISH, 15);
	this->fpga->readSGLResponseFIFO(this->sglBuffer, this->vmsApplicationSettings->NumberOfSensors * AXES_PER_SENSOR * MAX_SAMPLE_PER_PUBLISH, 25);
	if (this->vmsApplicationSettings->Subsystem == "M1M3") {
		this->processM1M3();
	}
	else if (this->vmsApplicationSettings->Subsystem == "M2") {
		this->processM2();
	}
	else if (this->vmsApplicationSettings->Subsystem == "TMA") {
		this->processTMA();
	}
}

void Accelerometer::processM1M3() {
	this->m1m3Data->Timestamp = Timestamp::fromRaw(this->u64Buffer[0]);
	int32_t dataBufferIndex = 0;
	for (int i = 0; i < MAX_SAMPLE_PER_PUBLISH; ++i) {
		this->m1m3Data->Sensor1XAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor1YAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor1ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor2XAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor2YAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor2ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor3XAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor3YAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor3ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor4XAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor4YAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor4ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor5XAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor5YAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor5ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor6XAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor6YAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
		this->m1m3Data->Sensor6ZAcceleration[i] = this->sglBuffer[dataBufferIndex];
		dataBufferIndex++;
	}
	this->publisher->putM1M3();
}

void Accelerometer::processM2() {
	this->m2Data->Timestamp = Timestamp::fromRaw(this->u64Buffer[0]);
	// TODO: Populate M2
	this->publisher->putM2();
}

void Accelerometer::processTMA() {
	this->tmaData->Timestamp = Timestamp::fromRaw(this->u64Buffer[0]);
	// TODO: Populate TMA
	this->publisher->putTMA();
}

} /* namespace VMS */
} /* namespace LSST */
