/*
 * SettingReader.cpp
 *
 *  Created on: Oct 3, 2017
 *      Author: ccontaxis
 */

#include <SettingReader.h>

namespace LSST {
namespace VMS {

SettingReader::SettingReader(std::string basePath) {
	this->basePath = basePath;
}

VMSApplicationSettings* SettingReader::loadVMSApplicationSettings() {
	this->vmsApplicationSettings.load(this->getBasePath("VMSApplicationSettings.xml").c_str());
	return &this->vmsApplicationSettings;
}

std::string SettingReader::getBasePath(std::string file) {
	return this->basePath + file;
}

} /* namespace VMS */
} /* namespace LSST */
