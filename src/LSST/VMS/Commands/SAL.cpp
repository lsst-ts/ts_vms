/*
 * SAL command.
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

#include <FPGA.h>

#include <Commands/SAL.h>
#include <Events/SummaryState.h>
#include <cRIO/ControllerThread.h>

#include <spdlog/spdlog.h>

using namespace LSST::cRIO;
using namespace LSST::VMS;
using namespace LSST::VMS::Commands;
using namespace MTVMS;

void SAL_start::execute() {
    SPDLOG_INFO("Starting, settings={}", params.configurationOverride);

    Events::SummaryState::setState(MTVMS_shared_SummaryStates_DisabledState);
    ackComplete();
    SPDLOG_INFO("Started");
}

void SAL_enable::execute() {
    Events::SummaryState::setState(MTVMS_shared_SummaryStates_EnabledState);
    ackComplete();
    SPDLOG_INFO("Enabled");
}

void SAL_disable::execute() {
    Events::SummaryState::setState(MTVMS_shared_SummaryStates_DisabledState);
    ackComplete();
}

void SAL_standby::execute() {
    Events::SummaryState::setState(MTVMS_shared_SummaryStates_StandbyState);
    ackComplete();
    SPDLOG_INFO("Standby");
}

void SAL_exitControl::execute() {
    ControllerThread::setExitRequested();
    ackComplete();
}

bool SAL_changeSamplePeriod::validate() {
    if (Events::SummaryState::instance().enabled() == false) {
        return false;
    }
    return true;
}

void SAL_changeSamplePeriod::execute() {
    FPGA::instance().setOperate(false);
    ackComplete();
}

void SAL_timeSynchronization::received() {
    std::cerr << "TimeSynchronization " << params.baseClockOffset << std::endl;
}
