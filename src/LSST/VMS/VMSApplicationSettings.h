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

#ifndef VMSAPPLICATIONSETTINGS_H_
#define VMSAPPLICATIONSETTINGS_H_

#include <string>
#include <vector>

namespace LSST {
namespace VMS {

class VMSApplicationSettings {
public:
    std::string Subsystem;
    bool IsController;
    int32_t period;
    int outputType;
    std::string RIO;
    std::vector<double> XCoefficients;
    std::vector<double> YCoefficients;
    std::vector<double> ZCoefficients;
    std::vector<double> XOffsets;
    std::vector<double> YOffsets;
    std::vector<double> ZOffsets;

    void load(const std::string &filename);
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* VMSAPPLICATIONSETTINGS_H_ */
