# define to "" to produce verbose output
ifndef VERBOSE
  co := @
  silence := --silence-errors
endif

c_opts := -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE

ifdef DEBUG
  c_opts += -g
else
  c_opts += -O3
endif

CRIOCPP := ../ts_cRIOcpp/

BOOST_CPPFLAGS := -I/usr/include/boost169
SAL_CPPFLAGS := $(shell pkg-config yaml-cpp --cflags $(silence)) \
        $(shell pkg-config fmt --cflags $(silence)) \
        $(shell pkg-config spdlog --cflags $(silence)) \
        $(shell pkg-config fftw3 --cflags $(silence)) \
        -I${OSPL_HOME}/include -I${OSPL_HOME}/include/sys \
        -I${OSPL_HOME}/include/dcps/C++/SACPP \
        -I${SAL_WORK_DIR}/MTVMS/cpp/src \
        -I${SAL_WORK_DIR}/include \
        -I${SAL_HOME}/include \
        -I${LSST_SDK_INSTALL}/include

LIBS := $(shell pkg-config yaml-cpp --libs $(silence)) \
        $(shell pkg-config fmt --libs $(silence)) \
        $(shell pkg-config fftw3 --libs $(silence)) \
        -ldl -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel \
        -lpthread -lddsserialization -lddsconfparser -lddsconf \
        -lddsdatabase -lddsutil -lddsos \
        ${SAL_WORK_DIR}/lib/libSAL_MTVMS.a \
        -lreadline

C := gcc -Wall ${c_opts}
CPP := g++ --std=c++17 -Wall -fPIE ${c_opts}

# Simulator is dynamicaly linked, distribution uses static linking
ifdef SIMULATOR
  CPP += -pedantic -DSIMULATOR
  LIBS += $(shell pkg-config spdlog --libs $(silence))
else
  C += -fmessage-length=0
  CPP += -fmessage-length=0 -ldl
  LIBS += $(shell pkg-config spdlog --libs $(silence) | sed -E 's/-l([a-z0-9]*)/-l:lib\1.a/g')
endif

LIBS_FLAGS += -L"${OSPL_HOME}/lib" -L"${LSST_SDK_INSTALL}/lib"

GIT := $(shell command -v git 2> /dev/null)

ifndef GIT
   VERSION := "v0.0.0-None"
   GIT_HASH := "xxxx-NONE"
else
   VERSION := $(shell git describe --tags --dirty)
   GIT_HASH := $(shell git rev-parse HEAD)
endif
