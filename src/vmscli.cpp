/*
 * Command line Vibration Monitoring System client.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <atomic>
#include <csignal>
#include <filesystem>

#include <cRIO/Application.h>
#include <cRIO/SimpleFPGACliApp.h>

#include <Events/FPGAState.h>
#include <FileAccelerometer.h>
#include <FPGA.h>
#include <SettingReader.h>
#include <VMSApplicationSettings.h>

using namespace LSST::cRIO;
using namespace LSST::VMS;

class VMScli : public SimpleFPGACliApp {
public:
    VMScli(const char* name, const char* description);

    void processArg(int opt, char* optarg) override;
    command_vec processArgs(int argc, char* const argv[]) override;

    int temperature(command_vec cmds);
    int record(command_vec cmds);

protected:
    virtual SimpleFPGA* newFPGA(const char* dir) override;

private:
    std::string _subsystem = "";
    VMSApplicationSettings _settings;
};

VMScli::VMScli(const char* name, const char* description) : SimpleFPGACliApp(name, description) {
    Events::FPGAState::instance().setPublish(false);

    addArgument('c', "<configuration path> use given configuration directory", ':');

    addCommand("temperature", std::bind(&VMScli::temperature, this, std::placeholders::_1), "", NEED_FPGA,
               NULL, "Reads cRIO temperature");

    addCommand("record", std::bind(&VMScli::record, this, std::placeholders::_1), "S", NEED_FPGA,
               "<filename>", "Record VMS values to a file");
}

void VMScli::processArg(int opt, char* optarg) {
    switch (opt) {
        case 'c':
            SPDLOG_DEBUG("Loading configuration from {}", optarg);
            SettingReader::instance().setRootPath(optarg);
            break;
        default:
            SimpleFPGACliApp::processArg(opt, optarg);
    }
}

command_vec VMScli::processArgs(int argc, char* const argv[]) {
    auto ret = SimpleFPGACliApp::processArgs(argc, argv);
    if (ret.size() != 1) {
        SPDLOG_CRITICAL(
                "Subsystem (M1M3, M2 or CameraRotator) needs to be "
                "provided on command line");
        exit(EXIT_FAILURE);
    }
    _subsystem = ret[0];
#ifdef SIMULATOR
    SPDLOG_INFO("Initiliaze simulator for {}", _subsystem);
#else
    SPDLOG_INFO("Initiliaze VMScli FPGA for {}", _subsystem);
#endif
    setDebugLevel(getDebugLevel());
    ret.erase(ret.begin());
    return ret;
}

int VMScli::temperature(command_vec cmds) {
    std::cout << "cRIO temperature is " << std::setprecision(2) << FPGA::instance().chassisTemperature()
              << "\u00b0C" << std::endl;
    return 0;
}

std::atomic<bool> recordVMS = false;

void sig_int(int signal) { recordVMS = false; }

int VMScli::record(command_vec cmds) {
    recordVMS = true;

    std::filesystem::path file_path(cmds[0]);

    std::cout << "Recording to " << file_path.string() << ", hit ctrl+c to end" << std::endl;

    auto previous = signal(SIGINT, sig_int);

    try {
        FileAccelerometer filea(&_settings, file_path);
        filea.enableAccelerometers();

        int counter = 21;

        while (recordVMS) {
            if (counter > 10) {
                std::cout << "Ticks: " << FPGA::instance().chassisTicks() << "\r";
                std::cout.flush();
                filea.flush();
                counter = 0;
            }
            for (int i = 0; i < _settings.sensors; i++) {
                filea.sampleData();
            }
            counter++;
        }

        filea.disableAccelerometers();

    } catch (const std::ios_base::failure& e) {
        std::cerr << std::endl << "Error writing to " << file_path.string() << ": " << e.what() << std::endl;
    }

    signal(SIGINT, previous);

    return 0;
}

SimpleFPGA* VMScli::newFPGA(const char* dir) {
    _settings = SettingReader::instance().loadVMSApplicationSettings(_subsystem);
    SPDLOG_INFO("Creating FPGA");
    FPGA::instance().populate(&_settings);

    return &FPGA::instance();
}

VMScli cli("VMS", "VMS Command Line Interface");

int main(int argc, char* const argv[]) { return cli.run(argc, argv); }
