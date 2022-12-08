/*
 * VMSSubscriber class.
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

#include <VMSSubscriber.h>
#include <Commands/SAL.h>

#include <cRIO/Command.h>
#include <cRIO/ControllerThread.h>
#include <SAL_MTVMS.h>
#include <spdlog/spdlog.h>

#include <chrono>

using namespace LSST::VMS;

constexpr int32_t ACK_INPROGRESS = 301;  /// Acknowledges command reception, command is being executed.
constexpr int32_t ACK_COMPLETE = 303;    /// Command is completed.
constexpr int32_t ACK_FAILED = -302;     /// Command execution failed.

VMSSubscriber::VMSSubscriber(std::shared_ptr<SAL_MTVMS> vmsSAL, std::shared_ptr<SAL_MTVMS> allvmsSAL) {
#define ADD_SAL_COMMAND(name)                                                                   \
    _commands[#name] = [vmsSAL]() {                                                             \
        MTVMS_command_##name##C data;                                                           \
        int32_t commandID = vmsSAL->acceptCommand_##name(&data);                                \
        if (commandID <= 0) return;                                                             \
        cRIO::ControllerThread::instance().enqueue(new Commands::SAL_##name(commandID, &data)); \
    }

    ADD_SAL_COMMAND(start);
    ADD_SAL_COMMAND(enable);
    ADD_SAL_COMMAND(disable);
    ADD_SAL_COMMAND(standby);
    ADD_SAL_COMMAND(exitControl);
    ADD_SAL_COMMAND(changeSamplePeriod);

#define ADD_SAL_EVENT(name)                                                               \
    _events[#name] = [allvmsSAL]() {                                                      \
        MTVMS_logevent_##name##C data;                                                    \
        int32_t result = allvmsSAL->getEvent_##name(&data);                               \
        if (result == SAL__NO_UPDATES) return;                                            \
        cRIO::ControllerThread::instance().enqueueEvent(new Commands::SAL_##name(&data)); \
    }

    ADD_SAL_EVENT(timeSynchronization);

    _commands["setLogLevel"] = [vmsSAL]() {
        MTVMS_command_setLogLevelC data;
        MTVMS_logevent_logLevelC newData;
        int32_t commandID = vmsSAL->acceptCommand_setLogLevel(&data);
        if (commandID <= 0) return;

        if (data.level >= 40) {
            spdlog::set_level(spdlog::level::err);
            newData.level = 40;
        } else if (data.level >= 30) {
            spdlog::set_level(spdlog::level::warn);
            newData.level = 30;
        } else if (data.level >= 20) {
            spdlog::set_level(spdlog::level::info);
            newData.level = 20;
        } else if (data.level >= 10) {
            spdlog::set_level(spdlog::level::debug);
            newData.level = 10;
        } else {
            spdlog::set_level(spdlog::level::trace);
            newData.level = 0;
        }
        vmsSAL->ackCommand_setLogLevel(commandID, ACK_COMPLETE, 0, (char *)"Complete");
        vmsSAL->logEvent_logLevel(&newData, 0);
    };

    // register all commands
    for (auto c : _commands) {
        SPDLOG_TRACE("Registering command {}", c.first);
        vmsSAL->salProcessor((char *)("MTVMS_command_" + c.first).c_str());
    }

    // register events
    for (auto e : _events) {
        SPDLOG_TRACE("Registering event {}", e.first);
        allvmsSAL->salTelemetrySub((char *)("MTVMS_logevent_" + e.first).c_str());
    }
}

VMSSubscriber::~VMSSubscriber() {}

void VMSSubscriber::run(std::unique_lock<std::mutex> &lock) {
    while (keepRunning) {
        tryCommands();
        tryEvents();
        runCondition.wait_for(lock, 100us);
    }
}

void VMSSubscriber::tryCommands() {
    for (auto c : _commands) {
        c.second();
    }
}

void VMSSubscriber::tryEvents() {
    for (auto e : _events) {
        e.second();
    }
}
