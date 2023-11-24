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

#include <VMSApplicationSettings.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <vector>

namespace LSST {
namespace VMS {

void VMSApplicationSettings::load(const std::string &filename) {
    SPDLOG_TRACE("VMSApplicationSettings({})", filename);
    try {
        YAML::Node doc = YAML::LoadFile(filename);

        Subsystem = doc["Subsystem"].as<std::string>();
        IsController = doc["IsController"].as<bool>();
        period = doc["Period"].as<int32_t>(5);
        outputType = doc["OutputType"].as<int>(3);
        RIO = doc["RIO"].as<std::string>("RIO0");
        XCoefficients = doc["XCoefficients"].as<std::vector<double>>();
        YCoefficients = doc["YCoefficients"].as<std::vector<double>>();
        ZCoefficients = doc["ZCoefficients"].as<std::vector<double>>();
        XOffsets = doc["XOffsets"].as<std::vector<double>>();
        YOffsets = doc["YOffsets"].as<std::vector<double>>();
        ZOffsets = doc["ZOffsets"].as<std::vector<double>>();
    } catch (YAML::Exception &ex) {
        SPDLOG_CRITICAL("YAML Loading {}: {}", filename, ex.what());
        exit(EXIT_FAILURE);
    }

    auto check_vector = [](const char *name, std::vector<double> vec) {
        if (vec.size() != 3) {
            SPDLOG_CRITICAL("Invalid array {} length: {}, expected 3", name, vec.size());
            exit(EXIT_FAILURE);
        }
    };

    check_vector("XCoefficients", XCoefficients);
    check_vector("YCoefficients", YCoefficients);
    check_vector("ZCoefficients", ZCoefficients);
    check_vector("XOffsets", XOffsets);
    check_vector("YOffsets", YOffsets);
    check_vector("ZOffsets", ZOffsets);
}

} /* namespace VMS */
} /* namespace LSST */
