//============================================================================
// Name        : ts_MTVMS.cpp
// Author      : LSST
// Version     :
// Copyright   : LSST
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <SettingReader.h>
#include <SAL_MTVMS.h>
#include <VMSPublisher.h>
#include <FPGA.h>
#include <Accelerometer.h>
#include <FPGAAddresses.h>
#include <Log.h>

#include <memory>

using namespace std;
using namespace LSST::VMS;

LSST::VMS::Logger Log;

int main() {
    Log.SetLevel(Levels::Info);
    Log.Info("Starting ts_VMS");
    Log.Info("Main: Creating setting reader");
    SettingReader settingReader = SettingReader("/usr/ts_VMS/SettingFiles/Base/");
    Log.Info("Main: Loading VMS application settings");
    VMSApplicationSettings* vmsApplicationSettings = settingReader.loadVMSApplicationSettings();
    Log.Info("\tSubsystem:       %s", vmsApplicationSettings->Subsystem.c_str());
    Log.Info("\tIsMaster:        %d", vmsApplicationSettings->IsMaster);
    Log.Info("\tNumberOfSensors: %d", vmsApplicationSettings->NumberOfSensors);
    Log.Info("Main: Initializing VMS SAL");
    std::shared_ptr<SAL_MTVMS> vmsSAL = std::make_shared<SAL_MTVMS>();
    vmsSAL->setDebugLevel(0);
    Log.Info("Main: Creating publisher");
    VMSPublisher publisher = VMSPublisher(vmsSAL);
    Log.Info("Main: Creating fpga");
    FPGA fpga = FPGA(vmsApplicationSettings);
    if (fpga.isErrorCode(fpga.initialize())) {
        Log.Fatal("Main: Error initializing FPGA");
        vmsSAL->salShutdown();
        return -1;
    }
    if (fpga.isErrorCode(fpga.open())) {
        Log.Fatal("Main: Error opening FPGA");
        vmsSAL->salShutdown();
        return -1;
    }
    Log.Info("Main: Creating accelerometer");
    Accelerometer accelerometer = Accelerometer(&publisher, &fpga, vmsApplicationSettings);

    // TODO: This is a non-commandable component so there isn't really a way to cleanly shutdown the software
    Log.Info("Main: Sample loop start");
    while (true) {
        fpga.waitForOuterLoopClock(105);
        Log.Trace("Main: Outer loop iteration start");
        accelerometer.sampleData();
        fpga.setTimestamp(publisher.getTimestamp());
        fpga.ackOuterLoopClock();
    }

    if (fpga.isErrorCode(fpga.close())) {
        Log.Error("Main: Error closing fpga");
    }

    if (fpga.isErrorCode(fpga.finalize())) {
        Log.Error("Main: Error finalizing fpga");
    }

    Log.Info("Main: Shutting down VMS SAL");
    vmsSAL->salShutdown();

    Log.Info("Main: Shutdown complete");

    return 0;
}
