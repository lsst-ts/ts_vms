/*
 * Generated with the FPGA Interface C API Generator 24.3
 * for NI-RIO 24.3 or later.
 */
#ifndef __NiFpga_VMS_6_Controller_h__
#define __NiFpga_VMS_6_Controller_h__

#ifndef NiFpga_Version
#define NiFpga_Version 243
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_VMS_6_Controller_Bitfile;
 */
#define NiFpga_VMS_6_Controller_Bitfile "NiFpga_VMS_6_Controller.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_VMS_6_Controller_Signature = "8935261293C28FA5CB2BE2ED9615D81B";

#if NiFpga_Cpp
extern "C" {
#endif

typedef enum {
    NiFpga_VMS_6_Controller_IndicatorBool_FIFOfull = 0x1804E,
    NiFpga_VMS_6_Controller_IndicatorBool_Ready = 0x1803E,
    NiFpga_VMS_6_Controller_IndicatorBool_Stopped = 0x18042,
    NiFpga_VMS_6_Controller_IndicatorBool_Timeouted = 0x18036
} NiFpga_VMS_6_Controller_IndicatorBool;

typedef enum { NiFpga_VMS_6_Controller_IndicatorU64_Ticks = 0x18038 } NiFpga_VMS_6_Controller_IndicatorU64;

typedef enum { NiFpga_VMS_6_Controller_ControlBool_Operate = 0x18032 } NiFpga_VMS_6_Controller_ControlBool;

typedef enum { NiFpga_VMS_6_Controller_ControlU32_Periodms = 0x18048 } NiFpga_VMS_6_Controller_ControlU32;

typedef enum {
    NiFpga_VMS_6_Controller_TargetToHostFifoSgl_Average = 3
} NiFpga_VMS_6_Controller_TargetToHostFifoSgl;

/* Indicator: ChassisTemperature */
static const NiFpga_FxpTypeInfo NiFpga_VMS_6_Controller_IndicatorFxp_ChassisTemperature_TypeInfo = {1, 32,
                                                                                                    16};

/* Use NiFpga_ReadU32() to access ChassisTemperature */
static const uint32_t NiFpga_VMS_6_Controller_IndicatorFxp_ChassisTemperature_Resource = 0x18044;

/* FIFO: Max */
static const NiFpga_FxpTypeInfo NiFpga_VMS_6_Controller_TargetToHostFifoFxp_Max_TypeInfo = {1, 24, 4};

/* Use NiFpga_ReadFifoU64() to access Max */
static const uint32_t NiFpga_VMS_6_Controller_TargetToHostFifoFxp_Max_Resource = 2;

/* FIFO: Min */
static const NiFpga_FxpTypeInfo NiFpga_VMS_6_Controller_TargetToHostFifoFxp_Min_TypeInfo = {1, 24, 4};

/* Use NiFpga_ReadFifoU64() to access Min */
static const uint32_t NiFpga_VMS_6_Controller_TargetToHostFifoFxp_Min_Resource = 1;

/* FIFO: RawOutput */
static const NiFpga_FxpTypeInfo NiFpga_VMS_6_Controller_TargetToHostFifoFxp_RawOutput_TypeInfo = {1, 24, 4};

/* Use NiFpga_ReadFifoU64() to access RawOutput */
static const uint32_t NiFpga_VMS_6_Controller_TargetToHostFifoFxp_RawOutput_Resource = 0;

#if NiFpga_Cpp
}
#endif

#endif
