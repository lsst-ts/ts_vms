# Vibration monitoring system (VMS)

This code shall run on VMS NI cRIOs. It loads a FPGA bitcode to read
accelerometers channels. Accelerometers are read and their values are available
at dedicated SAL channels.

## Operation

Raw signal is read from accelerometers attached at various telescope locations.
Multiple units are distributed through the telescope, with associated cRIO
reading them. Raw data are send over SAL/DDS to consumers, which either further
process them (create DDS spectra,..) and/or store raw/processed data.

## Building

VMS build depends on libraries from
[ts_cRIOcpp](https://github.com/lsst-ts/ts_cRIOcpp). Once ts_cRIOcpp is
compiled and LSST SAL/DDS infrastructure is setup, with libsacpp_MTVMS.a
available, build is trivial call to make:

```bash
make
```

*make deploy* target will install the binaries on cRIOs.
