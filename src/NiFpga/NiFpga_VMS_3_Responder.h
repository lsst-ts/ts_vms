/*
 * Generated with the FPGA Interface C API Generator 19.0
 * for NI-RIO 19.0 or later.
 */
#ifndef __NiFpga_VMS_3_Responder_h__
#define __NiFpga_VMS_3_Responder_h__

#ifndef NiFpga_Version
#define NiFpga_Version 190
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_VMS_3_Responder_Bitfile;
 */
#define NiFpga_VMS_3_Responder_Bitfile "NiFpga_VMS_3_Responder.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char *const NiFpga_VMS_3_Responder_Signature = "AF9CD1DCC916CE5298098C21EB399E23";

#if NiFpga_Cpp
extern "C" {
#endif

typedef enum {
    NiFpga_VMS_3_Responder_IndicatorBool_FIFOfull = 0x18032,
    NiFpga_VMS_3_Responder_IndicatorBool_Ready = 0x18026,
    NiFpga_VMS_3_Responder_IndicatorBool_Stopped = 0x1801A,
    NiFpga_VMS_3_Responder_IndicatorBool_Timeouted = 0x1801E,
} NiFpga_VMS_3_Responder_IndicatorBool;

typedef enum {
    NiFpga_VMS_3_Responder_IndicatorU64_Ticks = 0x18020,
} NiFpga_VMS_3_Responder_IndicatorU64;

typedef enum {
    NiFpga_VMS_3_Responder_ControlI16_Outputtype = 0x1802E,
} NiFpga_VMS_3_Responder_ControlI16;

typedef enum {
    NiFpga_VMS_3_Responder_ControlU32_Periodms = 0x18028,
} NiFpga_VMS_3_Responder_ControlU32;

typedef enum {
    NiFpga_VMS_3_Responder_TargetToHostFifoU32_ResponseFIFO = 0,
} NiFpga_VMS_3_Responder_TargetToHostFifoU32;

#if !NiFpga_VxWorks

/* Indicator: ChassisTemperature */
const NiFpga_FxpTypeInfo NiFpga_VMS_3_Responder_IndicatorFxp_ChassisTemperature_TypeInfo = {1, 32, 16};

/* Use NiFpga_ReadU32() to access ChassisTemperature */
const uint32_t NiFpga_VMS_3_Responder_IndicatorFxp_ChassisTemperature_Resource = 0x18034;

#endif /* !NiFpga_VxWorks */

#if NiFpga_Cpp
}
#endif

#endif
