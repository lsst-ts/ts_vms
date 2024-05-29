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

#include <cRIO/Application.h>
#include <cRIO/SimpleFPGACliApp.h>

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

protected:
    virtual SimpleFPGA* newFPGA(const char* dir) override;

private:
    std::string subsystem = "";
};

VMScli::VMScli(const char* name, const char* description) : SimpleFPGACliApp(name, description) {
    addArgument('c', "<configuration path> use given configuration directory", ':');

    addCommand("temperature", std::bind(&VMScli::temperature, this, std::placeholders::_1), "", NEED_FPGA,
               NULL, "Reads cRIO temperature");
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
    subsystem = ret[0];
#ifdef SIMULATOR
    SPDLOG_INFO("Initiliaze simulator for {}", subsystem);
#else
    SPDLOG_INFO("Initiliaze VMScli FPGA for {}", subsystem);
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

SimpleFPGA* VMScli::newFPGA(const char* dir) {
    VMSApplicationSettings settings = SettingReader::instance().loadVMSApplicationSettings(subsystem);
    SPDLOG_INFO("Creating FPGA");
    FPGA::instance().populate(&settings);
    return &FPGA::instance();
}

VMScli cli("VMS", "VMS Command Line Interface");

int main(int argc, char* const argv[]) { return cli.run(argc, argv); }
