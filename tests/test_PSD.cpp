/*
 * This file is part of LSST VMS test suite. Tests PSD Telemetry.
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

#include <algorithm>
#include <cmath>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <Telemetry/PSD.h>

using Catch::Approx;
using namespace LSST::VMS::Telemetry;

void test_peaks(std::vector<float> vec, std::vector<int> peaks, float margin = 10e-3, float min_psd = 1000) {
    for (auto p : peaks) {
        auto m = std::max_element(vec.begin(), vec.end());
        CHECK(*m > min_psd);
        CHECK(std::distance(vec.begin(), m) == p);
        *m = 0.0;
    }

    for (auto v : vec) {
        CHECK(v == Approx(0).margin(margin));
    }
}

TEST_CASE("Calculate PSD @ 200 Hz sampling", "[PSD]") {
    PSD psd;
    psd.configure(1, 100, 0.005);

    CHECK(psd.frequency(0) == 0);
    CHECK(psd.frequency(50) == 100.0);
    CHECK(psd.frequency(100) == 200.0);

    for (size_t i = 0; i < 50; i++) {
        CHECK(isnan(psd.accelerationPSDX[i]));
        CHECK(isnan(psd.accelerationPSDY[i]));
        CHECK(isnan(psd.accelerationPSDZ[i]));
    }

    for (float i = 0; i < psd.numDataPoints * 2; i++) {
        double theta = 2 * M_PI * i / psd.numDataPoints;
        psd.append(1.0 * sin(5 * theta) + 0.5 * cos(13 * theta), 1.0 * cos(40 * theta) + 0.5 * sin(2 * theta),
                   1.0 * sin(13.6 * theta) + 0.5 * cos(10.2 * theta) + 0.4 * sin(22 * theta), false);
    }

    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDX),
                                  std::begin(psd.accelerationPSDX) + psd.numDataPoints),
               {10, 26});
    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDY),
                                  std::begin(psd.accelerationPSDY) + psd.numDataPoints),
               {80, 4});
    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDZ),
                                  std::begin(psd.accelerationPSDZ) + psd.numDataPoints),
               {27, 44, 20, 21, 28, 26, 22, 25}, 100, 100);
}

TEST_CASE("Calculate PSD @ 50 Hz sampling", "[PSD]") {
    PSD psd;
    psd.configure(1, 200, 0.02);

    CHECK(psd.frequency(0) == 0);
    CHECK(psd.frequency(1) == 0.25);
    CHECK(psd.frequency(20) == 5);
    CHECK(psd.frequency(200) == 50);

    for (size_t i = 0; i < 50; i++) {
        CHECK(isnan(psd.accelerationPSDX[i]));
        CHECK(isnan(psd.accelerationPSDY[i]));
        CHECK(isnan(psd.accelerationPSDZ[i]));
    }

    for (float i = 0; i < psd.numDataPoints * 2; i++) {
        double theta = M_PI * i * 0.02;
        psd.append(1.0 * sin(5 * theta) + 0.5 * cos(13 * theta), 1.0 * cos(40 * theta) + 0.5 * sin(2 * theta),
                   1.0 * sin(13.6 * theta) + 0.5 * cos(10.2 * theta) + 0.4 * sin(22 * theta), false);
    }

    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDX),
                                  std::begin(psd.accelerationPSDX) + psd.numDataPoints),
               {20, 52});
    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDY),
                                  std::begin(psd.accelerationPSDY) + psd.numDataPoints),
               {160, 8});
    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDZ),
                                  std::begin(psd.accelerationPSDZ) + psd.numDataPoints),
               {54, 55, 41, 88, 53, 56, 52, 57, 42, 51, 40, 50, 58, 43, 49, 48, 59, 44, 47, 45, 46}, 100,
               100);
}
