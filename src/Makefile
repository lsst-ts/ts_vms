include ../Makefile.inc

all: libVMS.a

.PHONY: DEPS clean force

version.c: force
	${co}(grep -qsF "VERSION=\"$(VERSION)\"" version.c == 0 && echo "[VER] version.c $(VERSION) OK") || \
	( \
	   echo "[VER] version $(VERSION) git hash $(GIT_HASH)" && \
	   printf >version.c 'const char *VERSION="%s";\nconst char *GIT_HASH="%s";\nconst char *CONFIG_URL="https://github.com/lsst-ts/ts_config_mttcs/MTM1M3TS/v1";\nconst char *CONFIG_SCHEMA_VERSION="v1";' $(VERSION) $(GIT_HASH) \
        )

CPPFLAGS += -I. \
	-I"../$(CRIOCPP)/include" \
	-I"LSST/VMS" \
	-I"LSST/VMS/Include" \
	-I"LSST/VMS/Logging" \
	-I"LSST/VMS/Utility" \
	-I"NiFpga"

C_SRCS = $(shell find LSST NiFpga -name '*.c')
CPP_SRCS = $(shell find LSST NiFpga -name '*.cpp')

OBJS = $(patsubst %.c,%.c.o,$(C_SRCS)) $(patsubst %.cpp,%.cpp.o,$(CPP_SRCS)) version.c.o

C_DEPS = $(patsubst %.c,%.c.d,$(C_SRCS))
CPP_DEPS = $(patsubst %.cpp,%.cpp.d,$(CPP_SRCS))

ifneq ($(MAKECMDGOALS),clean)
  -include ${C_DEPS} ${CPP_DEPS}
endif

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
	${co}$(CPP) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(CPPFLAGS) -c -fmessage-length=0 -o $@ $(patsubst %.d,%,$<)

%.c.o: %.c.d
	@echo '[C  ] $(patsubst %.d,%,$<)'
	${co}$(C) -c -fmessage-length=0 -o $@ $(patsubst %.d,%,$<)

%.cpp.d: %.cpp
	@echo '[DPP] $<'
	${co}$(CPP) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(CPPFLAGS) -M $< -MF $@ -MT '$(patsubst %.cpp,%.o,$<) $@'

%.c.d: %.c
	@echo '[DEP] $<'
	${co}$(C) $(BOOST_CPPFLAGS) $(SAL_CPPFLAGS) $(CPPFLAGS) -M $< -MF $@ -MT '$(patsubst %.c,%.o,$<) $@'
