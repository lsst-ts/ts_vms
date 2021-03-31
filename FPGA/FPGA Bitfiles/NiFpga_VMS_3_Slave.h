/*
 * Generated with the FPGA Interface C API Generator 16.0.0
 * for NI-RIO 16.0.0 or later.
 */

#ifndef __NiFpga_VMS_3_Slave_h__
#define __NiFpga_VMS_3_Slave_h__

#ifndef NiFpga_Version
   #define NiFpga_Version 1600
#endif

#include "NiFpga.h"

/**
 * The filename of the FPGA bitfile.
 *
 * This is a #define to allow for string literal concatenation. For example:
 *
 *    static const char* const Bitfile = "C:\\" NiFpga_VMS_3_Slave_Bitfile;
 */
#define NiFpga_VMS_3_Slave_Bitfile "NiFpga_VMS_3_Slave.lvbitx"

/**
 * The signature of the FPGA bitfile.
 */
static const char* const NiFpga_VMS_3_Slave_Signature = "7FBC34B6984B2A470551241D25F0BE6D";

typedef enum
{
   NiFpga_VMS_3_Slave_TargetToHostFifoU64_U64ResponseFIFO = 0,
} NiFpga_VMS_3_Slave_TargetToHostFifoU64;

typedef enum
{
   NiFpga_VMS_3_Slave_TargetToHostFifoSgl_SGLResponseFIFO = 1,
} NiFpga_VMS_3_Slave_TargetToHostFifoSgl;

typedef enum
{
   NiFpga_VMS_3_Slave_HostToTargetFifoU16_CommandFIFO = 3,
   NiFpga_VMS_3_Slave_HostToTargetFifoU16_RequestFIFO = 2,
} NiFpga_VMS_3_Slave_HostToTargetFifoU16;

#endif
