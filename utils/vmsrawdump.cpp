/*
 * This file is part of LSST VMS package.
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

#if (defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)

#define __WINDOWS__

#endif

#if defined(__linux__) || defined(__CYGWIN__)

#include <endian.h>

#elif defined(__APPLE__)

#include <libkern/OSByteOrder.h>

#define be64toh(x) OSSwapBigToHostInt64(x)

#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN

#elif defined(__OpenBSD__)

#include <sys/endian.h>

#elif defined(__NetBSD__) || defined(__FreeBSD__) || defined(__DragonFly__)

#include <sys/endian.h>

#define be64toh(x) betoh64(x)

#elif defined(__WINDOWS__)

#include <windows.h>

#if BYTE_ORDER == LITTLE_ENDIAN

#if defined(_MSC_VER)
#include <stdlib.h>
#define be64toh(x) _byteswap_uint64(x)

#elif defined(__GNUC__) || defined(__clang__)

#define be64toh(x) __builtin_bswap64(x)
#else
#error platform not supported
#endif

#else

#error byte order not supported

#endif

#define __BYTE_ORDER BYTE_ORDER
#define __BIG_ENDIAN BIG_ENDIAN
#define __LITTLE_ENDIAN LITTLE_ENDIAN
#define __PDP_ENDIAN PDP_ENDIAN

#else

#error platform not supported

#endif

#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

using namespace std::chrono;

void dump_file(const char *filename) {
    std::ifstream file;

    try {
        // auto file = std::ifstream(argv[i], std::ifstream::in |
        // std::fstream::binary);
        file.open(filename, std::ifstream::binary);
        file.exceptions(std::ios::eofbit | std::ios::badbit | std::ios::failbit);

        // parse file header
        char header[4];

        file.read(header, 4);
        uint8_t len = header[3];
        header[3] = 0;
        if (strcmp(header, "VMS") != 0) {
            std::cerr << "File " << filename << " doesn't look like VMS dump file, skipping." << std::endl;
            return;
        }

        char subsystem[len];
        file.read(subsystem, len);

        uint8_t sensors = subsystem[len - 1];
        subsystem[len - 1] = 0;

        if (!(sensors == 3 || sensors == 6)) {
            std::cerr << "Expect to see 3 or 6 sensors in " << filename << ", saw " << +sensors << "."
                      << std::endl;
        }

        std::cout << subsystem << " time (UTC)";

        int i;

        for (i = 0; i < sensors; i++) {
            for (char ax = 'X'; ax <= 'Z'; ax++) {
                std::cout << ", " << +(i + 1) << ax;
            }
        }

        std::cout << std::endl;

        auto micros_d = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
        long int old_micros = 0;

        while (!file.eof()) {
            file.read(reinterpret_cast<char *>(&micros_d), sizeof(micros_d));

            if (old_micros >= micros_d) {
                std::cerr << "Recorded timestamps in file " << filename << " aren't increasing ("
                          << old_micros << " >= " << micros_d << "), skipping rest of the file!" << std::endl;
                return;
            }

            microseconds micros(micros_d);
            seconds sec = duration_cast<seconds>(micros);
            micros -= sec;

            system_clock::time_point tp{seconds(sec)};
            std::time_t ttp = system_clock::to_time_t(tp);
            auto time = gmtime(&ttp);
            std::printf("%04u-%02u-%02uT%02u:%02u:%02u.%06u", time->tm_year + 1900, time->tm_mon + 1,
                        time->tm_mday, time->tm_hour, time->tm_min, time->tm_sec,
                        static_cast<unsigned>(micros.count()));

            for (i = 0; i < sensors * 3; i++) {
                float value;
                file.read(reinterpret_cast<char *>(&value), sizeof(value));
                std::cout << ", " << value;
            }

            std::cout << std::endl;

            old_micros = micros_d;
        }
    } catch (const std::ios_base::failure &e) {
        if (file.eof()) {
            return;
        }
        std::cerr << "Cannot read raw DC accelerometer file " << filename << ": " << e.what() << std::endl;
    }
}

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cerr << "Decodes raw VMS accelerometer output. Needs at least one file "
                     "as an argument."
                  << std::endl;
        return -1;
    }

    for (int i = 1; i < argc; i++) {
        dump_file(argv[1]);
    }
}
