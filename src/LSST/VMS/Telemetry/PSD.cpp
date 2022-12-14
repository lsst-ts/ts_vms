/*
 * PSD telemetry handling class.
 *
 * Developed for the Vera C. Rubin Observatory Telescope & Site Software Systems.
 * This product includes software developed by the Vera C.Rubin Observatory Project
 * (https://www.lsst.org). See the COPYRIGHT file at the top-level directory of
 * this distribution for details of code ownership.
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

#include "PSD.h"

using namespace LSST::VMS::Telemetry;

PSD::PSD(short int _sensor) {
    timestamp = 0;
    interval = 1;
    sensor = _sensor;

    numDataPoints = 500;
    minPSDFrequency = 1;
    maxPSDFrequency = 50;

    for (int i = 0; i < 200; i++) {
        accelerationPSDX[i] = NAN;
        accelerationPSDY[i] = NAN;
        accelerationPSDZ[i] = NAN;
    }

    for (int i = 0; i < 3; i++) {
        _cache[i] = fftw_alloc_real(numDataPoints);
        _output[i] = fftw_alloc_complex(numDataPoints);
        _plans[i] = fftw_plan_dft_r2c_1d(numDataPoints, _cache[i], _output[i], FFTW_MEASURE);
    }

    _cache_size = 0;
}

PSD::~PSD(void) {
    for (int i = 0; i < 3; i++) {
        fftw_destroy_plan(_plans[i]);
        fftw_free(_output[i]);
        fftw_free(_cache[i]);
    }
}

void PSD::append(float x, float y, float z) {
    _cache[0][_cache_size] = x;
    _cache[1][_cache_size] = y;
    _cache[2][_cache_size] = z;

    _cache_size++;

    float *tels[3] = {accelerationPSDX, accelerationPSDY, accelerationPSDZ};

    if (_cache_size >= static_cast<std::make_unsigned_t<int>>(numDataPoints)) {
        for (int i = 0; i < 3; i++) {
            fftw_execute(_plans[i]);

            float *tel_out = tels[i];

            fftw_complex *cur_out = _output[i];
            for (int j = 0; j < numDataPoints && j < 200; j++, cur_out++, tel_out++) {
                *tel_out = std::norm(std::complex<double>((*cur_out)[0], (*cur_out)[1]));
            }
        }
        _cache_size = 0;
    }
}
