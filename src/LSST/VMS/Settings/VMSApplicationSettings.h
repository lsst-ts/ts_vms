/*
 * VMSApplicationSettings.h
 *
 *  Created on: Sep 29, 2017
 *      Author: ccontaxis
 */

#ifndef VMSAPPLICATIONSETTINGS_H_
#define VMSAPPLICATIONSETTINGS_H_

#include <string>
#include <vector>

namespace LSST {
namespace VMS {

class VMSApplicationSettings {
public:
    std::string Subsystem;
    bool IsMaster;
    std::string RIO;
    std::vector<double> XCoefficients;
    std::vector<double> YCoefficients;
    std::vector<double> ZCoefficients;
    std::vector<double> XOffsets;
    std::vector<double> YOffsets;
    std::vector<double> ZOffsets;

    void load(const std::string &filename);
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* VMSAPPLICATIONSETTINGS_H_ */
