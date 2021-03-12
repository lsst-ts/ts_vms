/*
 * Logger.h
 *
 *  Created on: Mar 6, 2018
 *      Author: ccontaxis
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <ctime>
#include <stdarg.h>
#include <stdio.h>

namespace LSST {
namespace VMS {

struct Levels {
  enum Type { Trace = 1, Debug = 2, Info = 3, Warn = 4, Error = 5, Fatal = 6 };
};

class Logger {
private:
  Levels::Type LOGGER_LEVEL;

public:
  void SetLevel(Levels::Type level);

  void Trace(const char *format, ...);
  void Debug(const char *format, ...);
  void Info(const char *format, ...);
  void Warn(const char *format, ...);
  void Error(const char *format, ...);
  void Fatal(const char *format, ...);

private:
  void log(const char *color, const char *level, const char *message);
};

} /* namespace VMS */
} /* namespace LSST */

#endif /* LOGGER_H_ */
