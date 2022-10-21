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
public:
    SettingReader(std::string _basePath);

    const VMSApplicationSettings loadVMSApplicationSettings();

private:
    std::string getBasePath(std::string file);

    VMSApplicationSettings vmsApplicationSettings;

    std::string basePath;
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* SETTINGREADER_H_ */
