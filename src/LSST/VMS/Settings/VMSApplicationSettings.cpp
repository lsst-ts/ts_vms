/*
 * VMSApplicationSettings.cpp
 *
 *  Created on: Sep 29, 2017
 *      Author: ccontaxis
 */

#include <VMSApplicationSettings.h>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>

#include <vector>

namespace LSST {
namespace VMS {

void VMSApplicationSettings::load(const std::string &filename) {
    SPDLOG_TRACE("VMSApplicationSettings({})", filename);
    try {
        YAML::Node doc = YAML::LoadFile(filename);

        Subsystem = doc["Subsystem"].as<std::string>();
        IsController = doc["IsController"].as<bool>();
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
