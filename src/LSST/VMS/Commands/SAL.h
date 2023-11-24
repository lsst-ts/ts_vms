/*
 * SAL commands
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

#ifndef _VMS_Command_SAL
#define _VMS_Command_SAL

#include <SAL_MTVMS.h>

#include <cRIO/SAL/Command.h>
#include <cRIO/SAL/Event.h>

#include <VMSPublisher.h>

namespace LSST {
namespace VMS {
namespace Commands {

SAL_COMMAND_CLASS(MTVMS, VMSPublisher::SAL(), start);

SAL_COMMAND_CLASS(MTVMS, VMSPublisher::SAL(), enable);

SAL_COMMAND_CLASS(MTVMS, VMSPublisher::SAL(), disable);

SAL_COMMAND_CLASS(MTVMS, VMSPublisher::SAL(), standby);

SAL_COMMAND_CLASS(MTVMS, VMSPublisher::SAL(), exitControl);

SAL_COMMAND_CLASS_validate(MTVMS, VMSPublisher::SAL(), changeSamplePeriod);

SAL_EVENT_CLASS(MTVMS, VMSPublisher::SAL(), timeSynchronization);

}  // namespace Commands
}  // namespace VMS
}  // namespace LSST

#endif  //! _VMS_Command_SAL
