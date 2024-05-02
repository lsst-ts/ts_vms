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

#include <spdlog/spdlog.h>

#include <VMSPublisher.h>

#include <Events/Heartbeat.h>

using namespace LSST::VMS::Events;

const float HEARTBEAT_PERIOD = 1.0;  //* Heartbeat period in seconds

Heartbeat::Heartbeat(token) { _lastToggleTimestamp = 0; }

void Heartbeat::tryToggle() {
    SPDLOG_TRACE("Trying to toggle heartbeat");
    double timestamp = VMSPublisher::instance().getTimestamp();
    if (timestamp >= (_lastToggleTimestamp + HEARTBEAT_PERIOD)) {
        SPDLOG_DEBUG("Toggling heartbeat");
        auto lag = timestamp - _lastToggleTimestamp;
        if (_lastToggleTimestamp != 0 && lag > HEARTBEAT_PERIOD * 1.1) {
            SPDLOG_WARN("Toggling heartbeat after {:0.03f} seconds!", lag);
        }

        heartbeat = !heartbeat;

        // sends software heartbeat
        VMSPublisher::instance().logHeartbeat(this);

        _lastToggleTimestamp = timestamp;
    }
}
