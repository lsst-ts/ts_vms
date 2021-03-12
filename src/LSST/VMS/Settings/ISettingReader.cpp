/*
 * ISettingReader.cpp
 *
 *  Created on: Oct 3, 2017
 *      Author: ccontaxis
 */

#include <ISettingReader.h>

namespace LSST {
namespace VMS {

ISettingReader::~ISettingReader() {}

VMSApplicationSettings *ISettingReader::loadVMSApplicationSettings() {
  return 0;
}

} /* namespace VMS */
} /* namespace LSST */
