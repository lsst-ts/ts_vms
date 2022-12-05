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

#ifndef _VMS__Commands_UPDATE_H_
#define _VMS__Commands_UPDATE_H_

#include <mutex>

#include <cRIO/Command.h>

namespace LSST {
namespace VMS {
namespace Commands {

/*!
 * This command is responsible for running a periodic update.
 * This is an internal command only and cannot be issued via SAL.
 */
class Update : public cRIO::Command {
public:
    Update() {}
    virtual ~Update() {}

    void execute() override;
};

} /* namespace Commands */
} /* namespace VMS */
} /* namespace LSST */

#endif /* _VMS__Commands_UPDATE_H_ */
