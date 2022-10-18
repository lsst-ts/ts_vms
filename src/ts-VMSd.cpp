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

#include <SettingReader.h>
#include <SAL_MTVMS.h>
#include <VMSPublisher.h>
#include <FPGA.h>
#include <Accelerometer.h>
#include <FPGAAddresses.h>

#include <cRIO/ControllerThread.h>
#include <cRIO/CSC.h>
#include <cRIO/NiError.h>
#include <cRIO/SALSink.h>
#include <cRIO/Settings/Path.h>

using namespace std::chrono;
using namespace LSST;
using namespace LSST::VMS;

class MTVMSd : public LSST::cRIO::CSC {
public:
    MTVMSd(const char* name, const char* description) : CSC(name, description) {}

protected:
    void processArg(int opt, char* optarg) override;
    void init() override;
    void done() override;
    int runLoop() override;

private:
    VMSApplicationSettings _vmsApplicationSettings;
    std::shared_ptr<SAL_MTVMS> _vmsSAL;
    Accelerometer* accelerometer;
};

SALSinkMacro(MTVMS);

int getIndex(const std::string subsystem) {
    const char* subsystems[] = {"M1M3", "M2", "CameraRotator", NULL};
    int index = 1;
    for (const char** s = subsystems; *s != NULL; s++, index++) {
        if (subsystem == *s) {
            return index;
        }
    }
    SPDLOG_CRITICAL("Unknown subsystem {}", subsystem);
    exit(EXIT_FAILURE);
}

FPGA* fpga = NULL;

void MTVMSd::processArg(int opt, char* optarg) {
    CSC::processArg(opt, optarg);

    SPDLOG_INFO("Setting root path {}", getConfigRoot());
    LSST::cRIO::Settings::Path::setRootPath(getConfigRoot());

#ifdef SIMULATOR
    SPDLOG_WARN("Starting ts_VMS simulator");
#else
    SPDLOG_INFO("Starting ts_VMS");
#endif

    SPDLOG_INFO("Main: Creating setting reader from {}", getConfigRoot());
    SettingReader settingReader = SettingReader(getConfigRoot());
    SPDLOG_DEBUG("Main: Loading VMS application settings");
    _vmsApplicationSettings = settingReader.loadVMSApplicationSettings();

    SPDLOG_INFO("Main: Creating FPGA");
    fpga = new FPGA(&_vmsApplicationSettings);
}

void MTVMSd::init() {
    int index = getIndex(_vmsApplicationSettings.Subsystem);
    SPDLOG_DEBUG("Subsystem: {}, Index: {}, IsMaster: {}, RIO: {}", _vmsApplicationSettings.Subsystem.c_str(),
                 index, _vmsApplicationSettings.IsMaster, _vmsApplicationSettings.RIO);

    SPDLOG_INFO("Initializing MTVMS SAL");
    _vmsSAL = std::make_shared<SAL_MTVMS>(index);
    _vmsSAL->setDebugLevel(getDebugLevelSAL());

    addSink(std::make_shared<SALSink_mt>(_vmsSAL));

    // spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->flush(); });

    accelerometer = new Accelerometer(fpga, &_vmsApplicationSettings);

    SPDLOG_INFO("Main: Setting publisher");
    VMSPublisher::instance().setSAL(_vmsSAL);

    fpga->setTimestamp(VMSPublisher::instance().getTimestamp());
    accelerometer->enableAccelerometers();
    std::this_thread::sleep_for(1000ms);

    daemonOK();
}

void MTVMSd::done() {
    accelerometer->disableAccelerometers();

    LSST::cRIO::ControllerThread::instance().stop();

    SPDLOG_INFO("Shutting down MTVMSd");
    removeSink();

    std::this_thread::sleep_for(10ms);

    SPDLOG_INFO("Main: Shutting down MTVMS SAL");
    _vmsSAL->salShutdown();

    delete accelerometer;
    accelerometer = NULL;
}

int MTVMSd::runLoop() {
    accelerometer->sampleData();
    fpga->setTimestamp(VMSPublisher::instance().getTimestamp());
    return LSST::cRIO::ControllerThread::exitRequested() ? 0 : 1;
}

int main(int argc, char* const argv[]) {
    MTVMSd csc("MTVMS", "Vibration Monitoring System CSC");

    csc.processArgs(argc, argv);

    try {
        csc.run(fpga);
    } catch (LSST::cRIO::NiError& nie) {
        SPDLOG_CRITICAL("Main: Error initializing ThermalFPGA: {}", nie.what());
    }

    delete fpga;
    fpga = NULL;

    return 0;
}
