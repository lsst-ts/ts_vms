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

#include <cRIO/NiError.h>
#include <cRIO/SALSink.h>

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/daily_file_sink.h"

#include <getopt.h>
#include <memory>
#include <signal.h>

using namespace std;
using namespace LSST;
using namespace LSST::VMS;

SALSinkMacro(MTVMS);

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

bool runLoop = true;

void sigKill(int signal) {
    SPDLOG_DEBUG("Kill/int signal received");
    runLoop = false;
}

std::vector<spdlog::sink_ptr> sinks;

void setSinks(std::string subsystem) {
    auto logger = std::make_shared<spdlog::async_logger>("MTVMS " + subsystem, sinks.begin(), sinks.end(),
                                                         spdlog::thread_pool(),
                                                         spdlog::async_overflow_policy::block);
    spdlog::set_default_logger(logger);
    spdlog::set_level((debugLevel == 0 ? spdlog::level::info
                                       : (debugLevel == 1 ? spdlog::level::debug : spdlog::level::trace)));
}

void processArgs(int argc, char* const argv[], const char*& configRoot) {
    int enabledSinks = 0x3;

    int opt;
    while ((opt = getopt(argc, argv, "bc:dfhsv")) != -1) {
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
                std::cerr << "Unknown option " << opt << std::endl;
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

    setSinks("init");
}

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

int main(int argc, char** argv) {
    const char* configRoot = "/usr/ts_VMS/SettingFiles/Base/";

    processArgs(argc, argv, configRoot);

#ifdef SIMULATOR
    SPDLOG_WARN("Starting ts_VMS simulator");
#else
    SPDLOG_INFO("Starting ts_VMS");
#endif

    SPDLOG_INFO("Main: Creating setting reader from {}", configRoot);
    SettingReader settingReader = SettingReader(configRoot);
    SPDLOG_INFO("Main: Loading VMS application settings");
    VMSApplicationSettings* vmsApplicationSettings = settingReader.loadVMSApplicationSettings();

    int index = getIndex(vmsApplicationSettings->Subsystem);
    SPDLOG_INFO("Subsystem: {}, Index: {}, IsMaster: {}, RIO: {}", vmsApplicationSettings->Subsystem.c_str(),
                index, vmsApplicationSettings->IsMaster, vmsApplicationSettings->RIO);

    SPDLOG_INFO("Main: Initializing VMS SAL");
    std::shared_ptr<SAL_MTVMS> vmsSAL = std::make_shared<SAL_MTVMS>(index);
    vmsSAL->setDebugLevel(0);

    sinks.push_back(std::make_shared<SALSink_mt>(vmsSAL));
    setSinks(vmsApplicationSettings->Subsystem);

    SPDLOG_INFO("Main: Setting publisher");
    VMSPublisher::instance().setSAL(vmsSAL);

    SPDLOG_INFO("Main: Creating fpga");
    FPGA fpga = FPGA(vmsApplicationSettings);

    try {
        fpga.initialize();
        fpga.open();
        SPDLOG_INFO("Main: Creating accelerometer");
        Accelerometer accelerometer = Accelerometer(&fpga, vmsApplicationSettings);

        signal(SIGKILL, sigKill);
        signal(SIGINT, sigKill);
        // TODO: This is a non-commandable component so there isn't really a way to cleanly shutdown the
        // software
        SPDLOG_INFO("Main: Sample loop start");
        while (runLoop) {
            fpga.waitForOuterLoopClock(105);
            SPDLOG_TRACE("Main: Outer loop iteration start");
            accelerometer.sampleData();
            fpga.setTimestamp(VMSPublisher::instance().getTimestamp());
            fpga.ackOuterLoopClock();
        }

        fpga.close();
        fpga.finalize();
    } catch (cRIO::NiError& nie) {
        SPDLOG_CRITICAL("Error starting or stopping FPG: {}", nie.what());
        fpga.finalize();
        vmsSAL->salShutdown();
        return -1;
    }

    SPDLOG_INFO("Main: Shutting down VMS SAL");
    sinks.pop_back();
    setSinks("done");
    usleep(1000);
    vmsSAL->salShutdown();

    SPDLOG_INFO("Main: Shutdown complete");

    return 0;
}
