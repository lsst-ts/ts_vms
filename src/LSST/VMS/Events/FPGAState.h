/*
 * AcquisitionPeriod event handling class.
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

#ifndef _VMS_Event_AcqusitionPeriod_
#define _VMS_Event_AcqusitionPeriod_

#include <SAL_MTVMS.h>
#include <cRIO/Singleton.h>

#include <VMSPublisher.h>

#include <mutex>
#include <spdlog/spdlog.h>

namespace LSST {
namespace VMS {
namespace Events {

/**
 * Acquistion Period handling class.
 */
class FPGAState final : MTVMS_logevent_fpgaStateC, public cRIO::Singleton<FPGAState> {
public:
    FPGAState(token);

    void setPeriodOutputType(int32_t newPeriod, int newOutputType);
    void setMisc(bool newReady, bool newTimeouted, bool newStopped, bool newFIFOFull);

    /***
     * Readout state from FPGA, sends updates if data were changed
     */
    void checkState();

    int32_t getPeriod() {
        std::lock_guard<std::mutex> lockG(_stateMutex);
        return period;
    }

    static void send();

private:
    std::mutex _stateMutex;
};

}  // namespace Events
}  // namespace VMS
}  // namespace LSST

#endif  //! _VMS_Event_AcqusitionPeriod_
