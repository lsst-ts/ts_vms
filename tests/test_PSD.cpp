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

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>

#include <Telemetry/PSD.h>

using Catch::Approx;
using namespace LSST::VMS::Telemetry;

void test_peaks(std::vector<float> vec, std::vector<int> peaks, float margin = 10e-3) {
    for (auto p : peaks) {
        auto m = std::max_element(vec.begin(), vec.end());
        REQUIRE(*m > 1000);
        REQUIRE(std::distance(vec.begin(), m) == p);
        vec[p] = 0.0;
    }

    for (auto v : vec) {
        REQUIRE(v == Approx(0).margin(margin));
    }
}

TEST_CASE("Calculate PSD", "[PSD]") {
    PSD psd(1);

    for (float i = 0; i < psd.numDataPoints; i++) {
        REQUIRE(isnan(psd.accelerationPSDX[0]));
        double theta = 2 * M_PI * i / psd.numDataPoints;
        psd.append(1.0 * sin(5 * theta) + 0.5 * cos(13 * theta),
                   1.0 * cos(40 * theta) + 0.5 * sin(50 * theta),
                   1.0 * sin(13.6 * theta) + 0.5 * cos(10.2 * theta) + 0.25 * sin(22 * theta));
    }

    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDX), std::end(psd.accelerationPSDX)), {5, 13});
    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDY), std::end(psd.accelerationPSDY)),
               {40, 50});
    test_peaks(std::vector<float>(std::begin(psd.accelerationPSDZ), std::end(psd.accelerationPSDZ)),
               {14, 10, 13, 22, 15, 12, 9, 16}, 900);
}
