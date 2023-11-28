/*
 * Generated with the FPGA Interface C API Generator 19.0
 * for NI-RIO 19.0 or later.
 */
#ifndef __NiFpga_VMS_CameraRotator_h__
#define __NiFpga_VMS_CameraRotator_h__

#ifndef NiFpga_Version
#define NiFpga_Version 190
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\"
 * NiFpga_VMS_CameraRotator_Bitfile;
 */
#define NiFpga_VMS_CameraRotator_Bitfile "NiFpga_VMS_CameraRotator.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char *const NiFpga_VMS_CameraRotator_Signature = "E93A26E996D3EC1D81D0C490AD5EE3D1";

#if NiFpga_Cpp
extern "C" {
#endif

typedef enum {
    NiFpga_VMS_CameraRotator_IndicatorBool_FIFOfull = 0x8132,
    NiFpga_VMS_CameraRotator_IndicatorBool_Ready = 0x8142,
    NiFpga_VMS_CameraRotator_IndicatorBool_Stopped = 0x812E,
    NiFpga_VMS_CameraRotator_IndicatorBool_Timeouted = 0x813E,
} NiFpga_VMS_CameraRotator_IndicatorBool;

typedef enum {
    NiFpga_VMS_CameraRotator_IndicatorU64_Ticks = 0x8128,
} NiFpga_VMS_CameraRotator_IndicatorU64;

typedef enum {
    NiFpga_VMS_CameraRotator_ControlBool_Operate = 0x8146,
} NiFpga_VMS_CameraRotator_ControlBool;

typedef enum {
    NiFpga_VMS_CameraRotator_ControlI16_Outputtype = 0x8136,
} NiFpga_VMS_CameraRotator_ControlI16;

typedef enum {
    NiFpga_VMS_CameraRotator_ControlU32_Periodms = 0x8138,
} NiFpga_VMS_CameraRotator_ControlU32;

typedef enum {
    NiFpga_VMS_CameraRotator_TargetToHostFifoU32_ResponseFIFO = 0,
} NiFpga_VMS_CameraRotator_TargetToHostFifoU32;

#if !NiFpga_VxWorks

/* Indicator: ChassisTemperature */
const NiFpga_FxpTypeInfo NiFpga_VMS_CameraRotator_IndicatorFxp_ChassisTemperature_TypeInfo = {1, 32, 16};

/* Use NiFpga_ReadU32() to access ChassisTemperature */
const uint32_t NiFpga_VMS_CameraRotator_IndicatorFxp_ChassisTemperature_Resource = 0x8124;

#endif /* !NiFpga_VxWorks */

#if NiFpga_Cpp
}
#endif

#endif
