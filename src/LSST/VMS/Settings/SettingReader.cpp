/*
 * SettingReader.cpp
 *
 *  Created on: Oct 3, 2017
 *      Author: ccontaxis
 */

#include <spdlog/spdlog.h>
#include <SettingReader.h>

namespace LSST {
namespace VMS {

SettingReader::SettingReader(std::string _basePath) {
    SPDLOG_INFO("SettingReader::SettingReader(\"{}\")", _basePath);
    basePath = _basePath;
}

VMSApplicationSettings *SettingReader::loadVMSApplicationSettings() {
    std::string applicationSettingsFile = getBasePath("/VMSApplicationSettings.yaml");
    SPDLOG_DEBUG("SettingReader: loadVMSApplicationSettings({})", applicationSettingsFile);
    vmsApplicationSettings.load(applicationSettingsFile);
    return &this->vmsApplicationSettings;
}

std::string SettingReader::getBasePath(std::string file) { return this->basePath + file; }

} /* namespace VMS */
} /* namespace LSST */
