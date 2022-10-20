/*
 * Thread to subscribe to other CSC.
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

#ifndef _VMS_VMSSubscriber_h
#define _VMS_VMSSubscriber_h

#include <map>
#include <functional>
#include <string>

#include <SAL_MTVMS.h>

#include <cRIO/Thread.h>

namespace LSST {
namespace VMS {

class VMSSubscriber : public cRIO::Thread {
public:
    VMSSubscriber(std::shared_ptr<SAL_MTVMS> vmsSAL, std::shared_ptr<SAL_MTVMS> allvmsSAL);
    virtual ~VMSSubscriber();

protected:
    void run(std::unique_lock<std::mutex>& lock) override;

private:
    std::map<std::string, std::function<void(void)>> _commands;
    std::map<std::string, std::function<void(void)>> _events;

    void tryCommands();
    void tryEvents();
};

}  // namespace VMS
}  // namespace LSST

#endif  //! _VMS_VMSSubscriber_h
