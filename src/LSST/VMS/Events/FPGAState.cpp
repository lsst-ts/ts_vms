/*
 * AcquistionPeriod event handling class.
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

#include <FPGA.h>
#include <Events/FPGAState.h>

#include <spdlog/fmt/fmt.h>

using namespace LSST::VMS;
using namespace LSST::VMS::Events;
using namespace MTVMS;

FPGAState::FPGAState(token) { period = 0; }

void FPGAState::send() {
    salReturn ret = VMSPublisher::SAL()->putSample_logevent_fpgaState(&instance());
    if (ret != SAL__OK) {
        SPDLOG_WARN("Can not send fpgaState: {}", ret);
        return;
    }
}

void FPGAState::setPeriodOutputType(int32_t newPeriod, int newOutputType) {
    std::lock_guard<std::mutex> lockG(_stateMutex);
    if (period == newPeriod && outputType == newOutputType) {
        return;
    }
    period = newPeriod;
    outputType = newOutputType;
    send();
}

void FPGAState::setMisc(bool newReady, bool newTimeouted, bool newStopped, bool newFIFOFull) {
    std::lock_guard<std::mutex> lockG(_stateMutex);
    if (ready == newReady && timeouted == newTimeouted && stopped == newStopped && fifoFull == newFIFOFull) {
        return;
    }
    ready = newReady;
    timeouted = newTimeouted;
    stopped = newStopped;
    fifoFull = newFIFOFull;
    send();
}

void FPGAState::checkState() {
    setMisc(FPGA::instance().ready(), FPGA::instance().timeouted(), FPGA::instance().stopped(),
            FPGA::instance().FIFOFull());
    send();
}
