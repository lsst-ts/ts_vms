//============================================================================
// Name        : ts_M1M3Support.cpp
// Author      : LSST
// Version     :
// Copyright   : LSST
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <SettingReader.h>
#include <SAL_vms.h>
#include <VMSPublisher.h>
#include <FPGA.h>
#include <Accelerometer.h>
#include <FPGAAddresses.h>
#include <iostream>

using namespace std;
using namespace LSST::VMS;

int main() {
	cout << "Starting ts_VMS" << endl;
	cout << "Creating setting reader" << endl;
	SettingReader settingReader = SettingReader("/usr/ts_VMS/SettingFiles/Base/");
	cout << "Loading VMS application settings" << endl;
	VMSApplicationSettings* vmsApplicationSettings = settingReader.loadVMSApplicationSettings();
	cout << "\tSubsystem: " << vmsApplicationSettings->Subsystem << endl;
	cout << "\tIsMaster: " << vmsApplicationSettings->IsMaster << endl;
	cout << "\tNumberOfSensors: " << vmsApplicationSettings->NumberOfSensors << endl;
	cout << "Initializing VMS SAL" << endl;
	SAL_vms vmsSAL = SAL_vms();
	vmsSAL.setDebugLevel(0);
	cout << "Creating publisher" << endl;
	VMSPublisher publisher = VMSPublisher(&vmsSAL);
	cout << "Creating fpga" << endl;
	FPGA fpga = FPGA(vmsApplicationSettings);
	if (fpga.isErrorCode(fpga.initialize())) {
		cout << "Error initializing FPGA" << endl;
		vmsSAL.salShutdown();
		return -1;
	}
	if (fpga.isErrorCode(fpga.open())) {
		cout << "Error opening FPGA" << endl;
		vmsSAL.salShutdown();
		return -1;
	}
	cout << "Creating accelerometer" << endl;
	Accelerometer accelerometer = Accelerometer(&publisher, &fpga, vmsApplicationSettings);

	cout << "Enabling accelerometers" << endl;
	accelerometer.enableAccelerometers();

	// TODO: This is a non-commandable component so there isn't really a way to cleanly shutdown the software
	while(true) {
		fpga.waitForOuterLoopClock(105);
		accelerometer.sampleData();
		fpga.setTimestamp(publisher.getTimestamp());
		fpga.ackOuterLoopClock();
	}

	cout << "Disabling accelerometers" << endl;
	accelerometer.disableAccelerometers();

	if (fpga.isErrorCode(fpga.close())) {
		cout << "Error closing fpga" << endl;
	}

	if (fpga.isErrorCode(fpga.finalize())) {
		cout << "Error finalizing fpga" << endl;
	}

	cout << "Shutting down VMS SAL" << endl;
	vmsSAL.salShutdown();

	cout << "Shutdown complete" << endl;

	return 0;
}
