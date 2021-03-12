/*
 * ISettingReader.h
 *
 *  Created on: Oct 3, 2017
 *      Author: ccontaxis
 */

#ifndef ISETTINGREADER_H_
#define ISETTINGREADER_H_

#include <string>

namespace LSST {
namespace VMS {

class VMSApplicationSettings;

/*!
 * Interface for reading settings.
 */
class ISettingReader {
public:
    virtual ~ISettingReader();

    /*!
     * Load the VMS application settings.
     * @return The VMS application settings
     */
    virtual VMSApplicationSettings *loadVMSApplicationSettings();
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* ISETTINGREADER_H_ */
