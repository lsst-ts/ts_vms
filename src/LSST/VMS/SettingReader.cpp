/*
 * This file is part of LSST MT VMS package.
 *
 * Developed for the Vera C. Rubin Telescope and Site System.
 * This product includes software developed by the LSST Project
 * (https://www.lsst.org).
 * See the COPYRIGHT file at the top-level directory of this distribution
 * for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <spdlog/spdlog.h>

#include <SettingReader.h>

namespace LSST {
namespace VMS {

void SettingReader::setRootPath(std::string rootPath) {
    SPDLOG_DEBUG("SettingReader: setRootPath(\"{}\")", rootPath);
    _rootPath = rootPath;
}

const VMSApplicationSettings SettingReader::loadVMSApplicationSettings(std::string subsystem) {
    std::string applicationSettingsFile = _getBasePath("/" + subsystem + ".yaml");
    SPDLOG_DEBUG("SettingReader: loadVMSApplicationSettings({})", applicationSettingsFile);
    _vmsApplicationSettings.load(applicationSettingsFile);
    return _vmsApplicationSettings;
}

std::string SettingReader::_getBasePath(std::string file) { return _rootPath + file; }

} /* namespace VMS */
} /* namespace LSST */
