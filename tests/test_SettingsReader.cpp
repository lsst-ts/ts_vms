/*
 * This file is part of LSST VMS test suite. Tests SafetyController.
 *
 * Developed for the LSST Telescope and Site Systems.
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

#include <catch2/catch_test_macros.hpp>

#include <SettingReader.h>

using namespace LSST::VMS;

TEST_CASE("List available settings", "[SettingReader]") {
    SettingReader::instance().setRootPath("../SettingFiles/");
    auto _vmsApplicationSettings = SettingReader::instance().loadVMSApplicationSettings("M1M3");
    REQUIRE(_vmsApplicationSettings.Subsystem == "M1M3");
}
