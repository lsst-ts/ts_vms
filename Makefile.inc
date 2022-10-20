# define to "" to produce verbose output
ifndef VERBOSE
  co := @
endif

c_opts :=  -fdata-sections -ffunction-sections -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE -DSAL_SUBSYSTEM_ID_IS_KEYED

ifdef DEBUG
  c_opts += -g
else
  c_opts += -O3
endif

ifdef SIMULATOR
  C := gcc -Wall ${c_opts}
  CPP := g++ -std=c++14 -pedantic -Wall -fmessage-length=0 ${c_opts} -DSIMULATOR
else
  C := gcc -Wall -fmessage-length=0 ${c_opts}
  CPP := g++ -std=c++14 -Wall -fmessage-length=0 ${c_opts}
endif

CRIOCPP := ../ts_cRIOcpp/

BOOST_CPPFLAGS := -I/usr/include/boost169
SAL_CPPFLAGS := -I${OSPL_HOME}/include -I${OSPL_HOME}/include/sys -I${OSPL_HOME}/include/dcps/C++/SACPP -I${SAL_WORK_DIR}/include -I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include

LIBS := -lyaml-cpp -ldl -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel -lpthread -lddsserialization -lddsconfparser -lddsconf -lddsdatabase -lddsutil -lddsos ${SAL_WORK_DIR}/lib/libSAL_MTVMS.a ${CRIOCPP}/lib/libcRIOcpp.a

LIBS_FLAGS += -L"${OSPL_HOME}/lib" -L"${LSST_SDK_INSTALL}/lib"

VERSION := $(shell git describe --tags --dirty)
ifeq ("$(VERSION)","")
  $(warning "Cannot retrieve version by git, using v0.0.0-None")
  VERSION := v0.0.0-None
endif
