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
 *    static const char* const Bitfile = "C:\\" NiFpga_VMS_CameraRotator_Bitfile;
 */
#define NiFpga_VMS_CameraRotator_Bitfile "NiFpga_VMS_CameraRotator.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_VMS_CameraRotator_Signature = "61F3E144C995010FBA8E37E32BA9B5D4";

#if NiFpga_Cpp
extern "C"
{
#endif

typedef enum
{
   NiFpga_VMS_CameraRotator_TargetToHostFifoU64_U64ResponseFIFO = 0,
} NiFpga_VMS_CameraRotator_TargetToHostFifoU64;

typedef enum
{
   NiFpga_VMS_CameraRotator_TargetToHostFifoSgl_SGLResponseFIFO = 1,
} NiFpga_VMS_CameraRotator_TargetToHostFifoSgl;

typedef enum
{
   NiFpga_VMS_CameraRotator_HostToTargetFifoU16_CommandFIFO = 3,
   NiFpga_VMS_CameraRotator_HostToTargetFifoU16_RequestFIFO = 2,
} NiFpga_VMS_CameraRotator_HostToTargetFifoU16;


#if NiFpga_Cpp
}
#endif

#endif
