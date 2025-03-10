/*
 * Update loop for VMS.
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

#include <Commands/Update.h>
#include <FPGA.h>
#include <VMSPublisher.h>

using namespace LSST::VMS::Commands;

LSST::cRIO::task_return_t Update::run() {
    VMSPublisher::instance().putMiscellaneous(FPGA::instance().chassisTemperature(),
                                              FPGA::instance().chassisTicks());
    return Task::DONT_RESCHEDULE;
}
