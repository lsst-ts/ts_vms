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

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/syslog_sink.h>

#include <chrono>
#include <getopt.h>
#include <fstream>
#include <grp.h>
#include <memory>
#include <pwd.h>
#include <signal.h>
#include <sys/types.h>

using namespace std::chrono;
using namespace LSST;
using namespace LSST::VMS;

SALSinkMacro(MTVMS);

void printHelp() {
    std::cout << "VMS controller. Runs on cRIO, reads out accelerometers signals." << std::endl
              << "Version: " << VERSION << std::endl
              << "  -b runs on background, don't log to stdout" << std::endl
              << "  -c <configuration path/device> use given configuration directory" << std::endl
              << "     (either absolute path, or just device name of configuration)" << std::endl
              << "  -d increases debugging (can be specified multiple times, default is info)" << std::endl
              << "  -f runs on foreground, don't log to file" << std::endl
              << "  -h prints this help" << std::endl
              << "  -p PID file, started as daemon on background" << std::endl
              << "  -s increases SAL debugging (can be specified multiple times, default is 0)" << std::endl
              << "  -u <user>:<group> run under user & group" << std::endl
              << "  -v prints version and exits" << std::endl;
    // << "  -V prints SAL, XML and OSPL versions and exits" << std::endl;
}

int debugLevel = 0;
int debugLevelSAL = 0;

bool runLoop = true;

const char* pidFile = NULL;
std::string daemonUser("vms");
std::string daemonGroup("vms");

int enabledSinks = 0x10;

void sigKill(int signal) {
    SPDLOG_DEBUG("Kill/int signal received");
    runLoop = false;
}

std::vector<spdlog::sink_ptr> sinks;

void setSinks(std::string subsystem) {
    auto logger = std::make_shared<spdlog::async_logger>("VMS " + subsystem, sinks.begin(), sinks.end(),
                                                         spdlog::thread_pool(),
                                                         spdlog::async_overflow_policy::block);
    spdlog::set_default_logger(logger);
    spdlog::set_level((debugLevel == 0 ? spdlog::level::info
                                       : (debugLevel == 1 ? spdlog::level::debug : spdlog::level::trace)));
}

void processArgs(int argc, char* const argv[], std::string& configRoot) {
    int opt;
    while ((opt = getopt(argc, argv, "bc:dfhp:su:v")) != -1) {
        switch (opt) {
            case 'b':
                enabledSinks |= 0x02;
                break;
            case 'c':
                if (optarg[0] == '/') {
                    configRoot = optarg;
                } else {
                    configRoot += optarg;
                }
                break;
            case 'd':
                debugLevel++;
                break;
            case 'f':
                enabledSinks |= 0x01;
                break;
            case 'h':
                printHelp();
                exit(EXIT_SUCCESS);
            case 'p':
                pidFile = optarg;
                enabledSinks |= 0x14;
                break;
            case 's':
                debugLevelSAL++;
                break;
            case 'u': {
                char* sep = strchr(optarg, ':');
                if (sep) {
                    *sep = '\0';
                    daemonUser = optarg;
                    daemonGroup = sep + 1;
                } else {
                    daemonGroup = daemonUser = optarg;
                }
                break;
            }
            case 'v':
                std::cout << VERSION << std::endl;
                exit(EXIT_SUCCESS);
// TODO enable once supported
#if 0
            case 'V':
                std::cout << "SAL " << SAL_MTVMS::getSALVersion() << std::endl
                          << "XML " << SAL_MTVMS::getXMLVersion() << std::endl
                          << "OSPL " << SAL_MTVMS::getOSPLVersion() << std::endl;
                exit(EXIT_SUCCESS);
#endif
            default:
                std::cerr << "Unknown option " << opt << std::endl;
                printHelp();
                exit(EXIT_FAILURE);
        }
    }
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

void startLog() {
    spdlog::init_thread_pool(8192, 1);
    if (enabledSinks & 0x01) {
        auto stdout_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        sinks.push_back(stdout_sink);
    }
    if (enabledSinks & 0x02) {
        auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("VMS", 0, 0);
        sinks.push_back(daily_sink);
    }
    if (enabledSinks & 0x04) {
        auto syslog_sink =
                std::make_shared<spdlog::sinks::syslog_sink_mt>("VMS", LOG_PID | LOG_CONS, LOG_USER, false);
        sinks.push_back(syslog_sink);
    }

    setSinks("init");
}

int main(int argc, char** argv) {
    std::string configRoot("/var/lib/ts-VMS/");

    processArgs(argc, argv, configRoot);

    int startPipe[2] = {-1, -1};

    if (pidFile) {
        struct passwd* runAs = getpwnam(daemonUser.c_str());
        if (runAs == NULL) {
            std::cerr << "Error: Cannot find user " << daemonUser << std::endl;
            exit(EXIT_FAILURE);
        }
        struct group* runGroup = getgrnam(daemonGroup.c_str());
        if (runGroup == NULL) {
            std::cerr << "Error: Cannot find group " << daemonGroup << std::endl;
            exit(EXIT_FAILURE);
        }

        if (pipe(startPipe) == -1) {
            std::cerr << "Error: Cannot create pipe for child/start process: " << strerror(errno)
                      << std::endl;
            exit(EXIT_FAILURE);
        }

        pid_t child = fork();
        if (child < 0) {
            std::cerr << "Error: Cannot fork:" << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
        if (child > 0) {
            close(startPipe[1]);
            std::ofstream pidf(pidFile, std::ofstream::out);
            pidf << child;
            pidf.close();
            if (pidf.fail()) {
                std::cerr << "Error: Cannot write to PID file " << pidFile << ": " << strerror(errno)
                          << std::endl;
                exit(EXIT_FAILURE);
            }
            char retbuf[2000];
            memset(retbuf, 0, sizeof(retbuf));
            signal(SIGALRM, [](int) {
                std::cerr << "Error: Start timeouted, see syslog for details." << std::endl;
                exit(EXIT_FAILURE);
            });
            alarm(15);
            read(startPipe[0], retbuf, 2000);
            if (strcmp(retbuf, "OK") == 0) {
                return EXIT_SUCCESS;
            }
            std::cerr << retbuf << std::endl;
            return EXIT_FAILURE;
        }
        close(startPipe[0]);
        startLog();
        setuid(runAs->pw_uid);
        setgid(runGroup->gr_gid);
        SPDLOG_DEBUG("Running as {}:{}", daemonUser, daemonGroup);
        if (!(enabledSinks & 0x01)) {
            close(0);
            close(1);
            close(2);
            int nf = open("/dev/null", O_RDWR);
            dup(nf);
            dup(nf);
            dup(nf);
        }
    } else {
        startLog();
    }

#ifdef SIMULATOR
    SPDLOG_WARN("Starting ts_VMS simulator");
#else
    SPDLOG_INFO("Starting ts_VMS");
#endif

    SPDLOG_INFO("Main: Creating setting reader from {}", configRoot);
    SettingReader settingReader = SettingReader(configRoot);
    SPDLOG_DEBUG("Main: Loading VMS application settings");
    VMSApplicationSettings* vmsApplicationSettings = settingReader.loadVMSApplicationSettings();

    int index = getIndex(vmsApplicationSettings->Subsystem);
    SPDLOG_DEBUG("Subsystem: {}, Index: {}, IsMaster: {}, RIO: {}", vmsApplicationSettings->Subsystem.c_str(),
                 index, vmsApplicationSettings->IsMaster, vmsApplicationSettings->RIO);

    SPDLOG_INFO("Main: Initializing VMS SAL");
    std::shared_ptr<SAL_MTVMS> vmsSAL = std::make_shared<SAL_MTVMS>(index);
    vmsSAL->setDebugLevel(debugLevelSAL);

    if (enabledSinks & 0x10) {
        sinks.push_back(std::make_shared<SALSink_mt>(vmsSAL));
        SPDLOG_INFO("Enabling SAL logger");
    }
    setSinks(vmsApplicationSettings->Subsystem);

    SPDLOG_INFO("Main: Setting publisher");
    VMSPublisher::instance().setSAL(vmsSAL);

    spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->flush(); });

    SPDLOG_INFO("Main: Creating fpga");
    FPGA fpga(vmsApplicationSettings);

    try {
        fpga.initialize();
        fpga.open();
        SPDLOG_INFO("Main: Creating accelerometer");
        Accelerometer accelerometer(&fpga, vmsApplicationSettings);

        signal(SIGKILL, sigKill);
        signal(SIGINT, sigKill);
        signal(SIGTERM, sigKill);

        std::this_thread::sleep_for(3000ms);

        fpga.setTimestamp(VMSPublisher::instance().getTimestamp());
        accelerometer.enableAccelerometers();
        std::this_thread::sleep_for(1000ms);

        // TODO: This is a non-commandable component so there isn't really a way to cleanly shutdown the
        // software
        SPDLOG_INFO("Main: Sample loop start");

        spdlog::apply_all([&](std::shared_ptr<spdlog::logger> l) { l->flush(); });

        if (startPipe[1] >= 0) {
            write(startPipe[1], "OK", 2);
            close(startPipe[1]);
        }

        while (runLoop) {
            SPDLOG_TRACE("Main: Outer loop iteration start");
            accelerometer.sampleData();
            fpga.setTimestamp(VMSPublisher::instance().getTimestamp());
        }

        accelerometer.disableAccelerometers();

        SPDLOG_INFO("Stopping FPGA");
        fpga.close();
        fpga.finalize();
    } catch (cRIO::NiError& nie) {
        if (startPipe[1] >= 0) {
            write(startPipe[1], nie.what(), strlen(nie.what()));
            close(startPipe[1]);
        }
        SPDLOG_CRITICAL("Error starting or stopping FPGA: {}", nie.what());
        fpga.close();
        fpga.finalize();
        vmsSAL->salShutdown();
        return -1;
    }

    SPDLOG_INFO("Main: Shutting down VMS SAL");
    if (enabledSinks & 0x10) {
        sinks.pop_back();
    }
    setSinks("done");
    std::this_thread::sleep_for(1000us);
    vmsSAL->salShutdown();

    std::this_thread::sleep_for(2s);

    SPDLOG_INFO("Main: Shutdown complete");

    if (pidFile) {
        unlink(pidFile);
    }

    return 0;
}
