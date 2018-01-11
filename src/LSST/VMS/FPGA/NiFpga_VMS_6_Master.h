/*
 * Generated with the FPGA Interface C API Generator 16.0.0
 * for NI-RIO 16.0.0 or later.
 */

#ifndef __NiFpga_VMS_6_Master_h__
#define __NiFpga_VMS_6_Master_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1600
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_VMS_6_Master_Bitfile;
 */
#define NiFpga_VMS_6_Master_Bitfile "NiFpga_VMS_6_Master.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_VMS_6_Master_Signature = "A13FAEB27C214060E11DAED8FC30F5C9";

typedef enum
{
   NiFpga_VMS_6_Master_TargetToHostFifoU64_U64ResponseFIFO = 0,
} NiFpga_VMS_6_Master_TargetToHostFifoU64;

typedef enum
{
   NiFpga_VMS_6_Master_TargetToHostFifoSgl_SGLResponseFIFO = 1,
} NiFpga_VMS_6_Master_TargetToHostFifoSgl;

typedef enum
{
   NiFpga_VMS_6_Master_HostToTargetFifoU16_CommandFIFO = 3,
   NiFpga_VMS_6_Master_HostToTargetFifoU16_RequestFIFO = 2,
} NiFpga_VMS_6_Master_HostToTargetFifoU16;

#endif
