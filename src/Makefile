include ../Makefile.inc

all: libVMS.a

.PHONY: DEPS clean

C_SRCS = $(shell find LSST NiFpga -name '*.c')
CPP_SRCS = $(shell find LSST NiFpga pugixml -name '*.cpp')

OBJS = $(patsubst %.c,%.c.o,$(C_SRCS)) $(patsubst %.cpp,%.cpp.o,$(CPP_SRCS))

C_DEPS = $(patsubst %.c,%.c.d,$(C_SRCS))
CPP_DEPS = $(patsubst %.cpp,%.cpp.d,$(CPP_SRCS))

ifneq ($(MAKECMDGOALS),clean)
  -include ${C_DEPS} ${CPP_DEPS}
endif

M1M3_CPPFLAGS := -I. \
	-I"LSST/VMS/Accelerometer" \
	-I"LSST/VMS/Include" \
	-I"LSST/VMS/Logging" \
	-I"LSST/VMS/FPGA" \
	-I"LSST/VMS/Publisher" \
	-I"LSST/VMS/Settings" \
	-I"LSST/VMS/Utility"

libVMS.a: $(OBJS)
	@echo '[AR ] $@'
	${co}$(AR) r $@ $^
	@echo '[RLB] $@'
	${co}ranlib $@

clean:
	@$(foreach file,$(OBJS) $(C_DEPS) $(CPP_DEPS) libVMS.a $(shell find -name '*.d'), echo '[RM ] ${file}'; $(RM) -r $(file);)

# file targets
%.cpp.o: %.cpp.d
	@echo '[CPP] $(patsubst %.d,%,$<)'
	${co}$(CPP) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(M1M3_CPPFLAGS) -c -fmessage-length=0 -o $@ $(patsubst %.d,%,$<)

%.c.o: %.c.d
	@echo '[C  ] $(patsubst %.d,%d,$<)'
	${co}$(C) -c -fmessage-length=0 -o $@ $(patsubst %.d,%,$<)

%.cpp.d: %.cpp
	@echo '[DPP] $<'
	${co}$(CPP) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(M1M3_CPPFLAGS) -M $< -MF $@ -MT '$(patsubst %.cpp,%.o,$<) $@'

%.c.d: %.c
	@echo '[DEP] $<'
	${co}$(C) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(M1M3_CPPFLAGS) -M $< -MF $@ -MT '$(patsubst %.c,%.o,$<) $@'
