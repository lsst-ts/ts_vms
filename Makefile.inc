# define to "" to produce verbose output
ifndef VERBOSE
  co := @
endif

c_opts := -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE

ifdef DEBUG
  c_opts += -g
else
  c_opts += -O3
endif

ifdef SIMULATOR
  C := gcc -Wall ${c_opts}
  CPP := g++ -std=c++14 -pedantic -Wall ${c_opts} -DSIMULATOR
else
  C := gcc -Wall -fmessage-length=0 ${c_opts}
  CPP := g++ -std=c++14 -Wall -fmessage-length=0 ${c_opts}
endif

CRIOCPP := ../ts_cRIOcpp/

BOOST_CPPFLAGS := -I/usr/include/boost169
SAL_CPPFLAGS := $(shell pkg-config yaml-cpp --cflags) -I${OSPL_HOME}/include -I${OSPL_HOME}/include/sys -I${OSPL_HOME}/include/dcps/C++/SACPP -I${SAL_WORK_DIR}/MTVMS/cpp/src -I${SAL_WORK_DIR}/include -I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include

LIBS := $(shell pkg-config yaml-cpp --libs) -ldl -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel -lpthread -lddsserialization -lddsconfparser -lddsconf -lddsdatabase -lddsutil -lddsos ${SAL_WORK_DIR}/lib/libSAL_MTVMS.a

LIBS_FLAGS += -L"${OSPL_HOME}/lib" -L"${LSST_SDK_INSTALL}/lib"

GIT := $(shell command -v git 2> /dev/null)

ifndef GIT
   VERSION := "v0.0.0-None"
   GIT_HASH := "xxxx-NONE"
else
   VERSION := $(shell git describe --tags --dirty)
   GIT_HASH := $(shell git rev-parse HEAD)
endif
