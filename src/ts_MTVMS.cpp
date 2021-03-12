//============================================================================
// Name    : ts_MTVMS.cpp
// Author    : LSST
// Version   :
// Copyright   : LSST
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <SettingReader.h>
#include <SAL_MTVMS.h>
#include <VMSPublisher.h>
#include <FPGA.h>
#include <Accelerometer.h>
#include <FPGAAddresses.h>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

#include <getopt.h>
#include <memory>

using namespace std;
using namespace LSST::VMS;

void printHelp() {
    std::cout << "VMS controller. Runs on cRIO, reads out accelerometers signals." << std::endl
              << "Version: " << VERSION << std::endl
              << "  -b runs on background, don't log to stdout" << std::endl
              << "  -c <configuration path> use given configuration directory (should be SettingFiles)"
              << std::endl
              << "  -d increases debugging (can be specified multiple times, default is info)" << std::endl
              << "  -f runs on foreground, don't log to file" << std::endl
              << "  -h prints this help" << std::endl
              << "  -s increases SAL debugging (can be specified multiple times, default is 0)" << std::endl
              << "  -v prints version and exits" << std::endl;
}

int debugLevel = 0;
int debugLevelSAL = 0;

std::vector<spdlog::sink_ptr> sinks;

void processArgs(int argc, char* const argv[], const char*& configRoot) {
    int enabledSinks = 0x3;

    int opt;
    while ((opt = getopt(argc, argv, "bc:fhsv")) != -1) {
        switch (opt) {
            case 'b':
                enabledSinks &= ~0x01;
                break;
            case 'c':
                configRoot = optarg;
                break;
            case 'd':
                debugLevel++;
                break;
            case 'f':
                enabledSinks &= ~0x02;
                break;
            case 'h':
                printHelp();
                exit(EXIT_SUCCESS);
            case 's':
                debugLevelSAL++;
                break;
            case 'v':
                std::cout << VERSION << std::endl;
                exit(EXIT_SUCCESS);
            default:
                std::cerr << "Unknow option " << opt << std::endl;
                printHelp();
                exit(EXIT_FAILURE);
        }
    }

    spdlog::init_thread_pool(8192, 1);
    if (enabledSinks & 0x01) {
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.push_back(stdout_sink);
    }
    if (enabledSinks & 0x02) {
        auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("MTM1M3", 0, 0);
        sinks.push_back(daily_sink);
    }
    auto logger = std::make_shared<spdlog::async_logger>("MTM1M3", sinks.begin(), sinks.end(),
                                                         spdlog::thread_pool(),
                                                         spdlog::async_overflow_policy::block);
    spdlog::set_default_logger(logger);
    spdlog::set_level((debugLevel == 0 ? spdlog::level::info
                                       : (debugLevel == 1 ? spdlog::level::debug : spdlog::level::trace)));
}

int main(int argc, char** argv) {
    const char* configRoot = "/usr/ts_VMS/SettingFiles/Base/";

    processArgs(argc, argv, configRoot);

    SPDLOG_INFO("Starting ts_VMS");
    SPDLOG_INFO("Main: Creating setting reader");
    SettingReader settingReader = SettingReader(configRoot);
    SPDLOG_INFO("Main: Loading VMS application settings");
    VMSApplicationSettings* vmsApplicationSettings = settingReader.loadVMSApplicationSettings();
    SPDLOG_INFO("\tSubsystem:     %s", vmsApplicationSettings->Subsystem.c_str());
    SPDLOG_INFO("\tIsMaster:    %d", vmsApplicationSettings->IsMaster);
    SPDLOG_INFO("\tNumberOfSensors: %d", vmsApplicationSettings->NumberOfSensors);
    SPDLOG_INFO("Main: Initializing VMS SAL");
    std::shared_ptr<SAL_MTVMS> vmsSAL = std::make_shared<SAL_MTVMS>();
    vmsSAL->setDebugLevel(0);
    SPDLOG_INFO("Main: Creating publisher");
    VMSPublisher publisher = VMSPublisher(vmsSAL);
    SPDLOG_INFO("Main: Creating fpga");
    FPGA fpga = FPGA(vmsApplicationSettings);
    if (fpga.isErrorCode(fpga.initialize())) {
        SPDLOG_CRITICAL("Main: Error initializing FPGA");
        vmsSAL->salShutdown();
        return -1;
    }
    if (fpga.isErrorCode(fpga.open())) {
        SPDLOG_CRITICAL("Main: Error opening FPGA");
        vmsSAL->salShutdown();
        return -1;
    }
    SPDLOG_INFO("Main: Creating accelerometer");
    Accelerometer accelerometer = Accelerometer(&publisher, &fpga, vmsApplicationSettings);

    // TODO: This is a non-commandable component so there isn't really a way to cleanly shutdown the software
    SPDLOG_INFO("Main: Sample loop start");
    while (true) {
        fpga.waitForOuterLoopClock(105);
        SPDLOG_TRACE("Main: Outer loop iteration start");
        accelerometer.sampleData();
        fpga.setTimestamp(publisher.getTimestamp());
        fpga.ackOuterLoopClock();
    }

    if (fpga.isErrorCode(fpga.close())) {
        SPDLOG_ERROR("Main: Error closing fpga");
    }

    if (fpga.isErrorCode(fpga.finalize())) {
        SPDLOG_ERROR("Main: Error finalizing fpga");
    }

    SPDLOG_INFO("Main: Shutting down VMS SAL");
    vmsSAL->salShutdown();

    SPDLOG_INFO("Main: Shutdown complete");

    return 0;
}
