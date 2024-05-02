/*
 * This file is part of the LSST M1M3 thermal system package.
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

#ifndef LSST_HEARTBEAT_H
#define LSST_HEARTBEAT_H

#include <chrono>

#include <SAL_MTVMS.h>

#include <cRIO/Singleton.h>

namespace LSST {
namespace VMS {
namespace Events {

/**
 * Wrapper object for MTM1M3_logevent_heartbeatStatusC.
 */
class Heartbeat : public MTVMS_logevent_heartbeatC, public cRIO::Singleton<Heartbeat> {
public:
    /**
     * Construct new InterlockStatus
     */
    Heartbeat(token);

    /**
     * Sets heartbeat, publish data if the last heartbeat was send more than 500ms
     * in past.
     */
    void tryToggle();

private:
    double _lastToggleTimestamp;
};

}  // namespace Events
}  // namespace VMS
}  // namespace LSST

#endif  // LSST_INTERLOCKSTATUS_H
