/*
 * This file is part of LSST MT VMS package.
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

#ifndef FPGA_H_
#define FPGA_H_

#include <NiFpga.h>

#include <cRIO/SimpleFPGA.h>
#include <cRIO/Singleton.h>

namespace LSST {
namespace VMS {

const NiFpga_FxpTypeInfo ResponseFxpTypeInfo = {1, 24, 8};

class VMSApplicationSettings;

/*!
 * The class used to communicate with the FPGA.
 */
class FPGA : public LSST::cRIO::SimpleFPGA, public LSST::cRIO::Singleton<FPGA> {
public:
    FPGA(token);

    void populate(VMSApplicationSettings *vmsApplicationSettings);

    void initialize() override;
    void open() override;
    void close() override;
    void finalize() override;

    float chassisTemperature();
    uint64_t chassisTicks();
    void setOperate(bool operate);
    void setPeriodOutputType(uint32_t period, int16_t outputType);

    bool ready();
    bool timeouted();
    bool stopped();
    bool FIFOFull();

    void readResponseFIFO(uint32_t *data, size_t length, int32_t timeoutInMs);

private:
    VMSApplicationSettings *_vmsApplicationSettings;
    uint32_t session;
    size_t remaining;
    NiFpga_IrqContext outerLoopIRQContext;
    uint8_t _channels;
    const char *_bitFile;
    const char *_signature;
    uint32_t _responseFIFO;
    uint32_t _chasisTemperatureResource;
    NiFpga_FxpTypeInfo _chasisTemperatureTypeInfo;
    uint32_t _operateResource;
    uint32_t _readyResource;
    uint32_t _periodResource;
    uint32_t _outputTypeResource;
    uint32_t _stoppedResource;
    uint32_t _timeoutedResource;
    uint32_t _fifoFullResource;
    uint32_t _ticksResource;
};

}  // namespace VMS
}  // namespace LSST

#endif /* FPGA_H_ */
