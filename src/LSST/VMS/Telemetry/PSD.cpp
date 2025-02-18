/*
 * PSD telemetry handling class.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software
 * Systems. This product includes software developed by the Vera C.Rubin
 * Observatory Project (https://www.lsst.org). See the COPYRIGHT file at the
 * top-level directory of this distribution for details of code ownership.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <spdlog/fmt/fmt.h>

#include "PSD.h"
#include "VMSPublisher.h"

// length of PSD array is defined in XML
constexpr size_t MAX_DATAPOINTS = 200;

using namespace LSST::VMS::Telemetry;

PSD::PSD() {
    timestamp = 0;
    interval = 1;
    sensor = -1;

    numDataPoints = 0;
    interval = NAN;
    _samplingPeriod = NAN;
    minPSDFrequency = NAN;
    maxPSDFrequency = NAN;

    _clearPSDs();

    for (int i = 0; i < 3; i++) {
        _cache[i] = NULL;
        _output[i] = NULL;
        _plans[i] = NULL;
    }

    _cache_size = 0;
}

PSD::~PSD(void) {
    for (int i = 0; i < 3; i++) {
        if (_plans[i] != NULL) {
            fftw_destroy_plan(_plans[i]);
            _plans[i] = NULL;
        }
        if (_output[i] != NULL) {
            fftw_free(_output[i]);
            _output[i] = NULL;
        }
        if (_cache[i] != NULL) {
            fftw_free(_cache[i]);
            _cache[i] = NULL;
        }
    }
}

void PSD::configure(short int sensorId, size_t dataPoints, float samplingPeriod) {
    sensor = sensorId;
    // maximal number of datapoints is defined in XML
    numDataPoints = std::min(dataPoints, MAX_DATAPOINTS);
    _samplingPeriod = samplingPeriod;
    interval = _samplingPeriod * dataPoints;
    minPSDFrequency = frequency(0);
    maxPSDFrequency = frequency(numDataPoints);
    _cache_size = 0;

    for (int i = 0; i < 3; i++) {
        if (_cache[i] != NULL) {
            fftw_free(_cache[i]);
        }
        _cache[i] = fftw_alloc_real(numDataPoints * 2);

        if (_output[i] != NULL) {
            fftw_free(_output[i]);
        }
        _output[i] = fftw_alloc_complex(numDataPoints + 1);

        if (_plans[i] != NULL) {
            fftw_free(_plans[i]);
        }
        _plans[i] = fftw_plan_dft_r2c_1d(numDataPoints * 2, _cache[i], _output[i], FFTW_MEASURE);
    }
}

void PSD::append(float x, float y, float z) {
    if (_cache[0] == NULL) {
        throw std::runtime_error(fmt::format("unconfigured PSD for sensor {}", sensor));
    }
    _cache[0][_cache_size] = x;
    _cache[1][_cache_size] = y;
    _cache[2][_cache_size] = z;

    _cache_size++;

    if (_cache_size >= static_cast<size_t>(numDataPoints) * 2) {
        float *tels[3] = {accelerationPSDX.data(), accelerationPSDY.data(), accelerationPSDZ.data()};
        for (int i = 0; i < 3; i++) {
            fftw_execute(_plans[i]);

            float *tel_out = tels[i];

            fftw_complex *cur_out = _output[i];
            for (int j = 0; j < numDataPoints; j++, cur_out++, tel_out++) {
                *tel_out = std::norm(std::complex<double>((*cur_out)[0], (*cur_out)[1]));
            }
        }

        VMSPublisher::instance().putPsd(this);
        _clearPSDs();

        _cache_size = 0;
    }
}

float PSD::frequency(size_t index) { return index * (1 / (2.0f * _samplingPeriod)) / numDataPoints; }

void PSD::_clearPSDs() {
    for (size_t i = 0; i < MAX_DATAPOINTS; i++) {
        accelerationPSDX[i] = NAN;
        accelerationPSDY[i] = NAN;
        accelerationPSDZ[i] = NAN;
    }
}
