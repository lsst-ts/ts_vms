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
    SPDLOG_DEBUG("SettingReader: loadVMSApplicationSettings()");
    vmsApplicationSettings.load(getBasePath("VMSApplicationSettings.xml").c_str());
    return &this->vmsApplicationSettings;
}

std::string SettingReader::getBasePath(std::string file) { return this->basePath + file; }

} /* namespace VMS */
} /* namespace LSST */
