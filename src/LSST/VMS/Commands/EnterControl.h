/*
 * EnterControl command.
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

#ifndef _VMS_Command_EnterControl_
#define _VMS_Command_ENterControl_

#include <SAL_MTVMS.h>
#include <VMSPublisher.h>
#include <cRIO/Command.h>

#include <Events/SummaryState.h>

namespace LSST {
namespace VMS {
namespace Commands {

class EnterControl : public cRIO::Command {
public:
    void execute() override {
        SPDLOG_DEBUG("EnterControl");
        VMSPublisher::instance().logSoftwareVersions();
        VMSPublisher::instance().logSimulationMode();
        Events::SummaryState::setState(MTVMS::MTVMS_shared_SummaryStates_StandbyState);
    }
};

}  // namespace Commands
}  // namespace VMS
}  // namespace LSST

#endif  // !_VMS_Command_EnterControl_
