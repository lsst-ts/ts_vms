/*
 * SettingReader.h
 *
 *  Created on: Oct 3, 2017
 *      Author: ccontaxis
 */

#ifndef SETTINGREADER_H_
#define SETTINGREADER_H_

#include <VMSApplicationSettings.h>
#include <string>

namespace LSST {
namespace VMS {

class SettingReader {
private:
    VMSApplicationSettings vmsApplicationSettings;

    std::string basePath;

public:
    SettingReader(std::string _basePath);

    VMSApplicationSettings *loadVMSApplicationSettings();

private:
    std::string getBasePath(std::string file);
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* SETTINGREADER_H_ */
