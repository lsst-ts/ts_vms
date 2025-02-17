# define to "" to produce verbose output
#define VERBOSE ""

ifndef VERBOSE
  co := @
  silence := --silence-errors
endif

# change as needed
ifeq ($(WITH_SAL_KAFKA),1)
  c_opts := -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE -DWITH_SAL_KAFKA
else
  c_opts := -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
endif

ifdef DEBUG
  c_opts += -g
  silence := --silence-errors
else
  c_opts += -O3
endif

CRIOCPP := ../ts_cRIOcpp/

PKG_CPPFLAGS := $(shell pkg-config yaml-cpp spdlog fmt fftw3 --cflags $(silence))

ifdef WITH_SAL_KAFKA
  SAL_CPPFLAGS += $(PKG_CPPFLAGS)  \
        -I${SAL_WORK_DIR}/MTVMS/cpp/src \
        -I${SAL_WORK_DIR}/include -I${CRIOCPP} -I. \
        -I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include -I${SAL_HOME}/include -I${LSST_SAL_PREFIX}/include -I${LSST_SAL_PREFIX}/include/avro
  LIBS += $(PKG_LIBS) -ldl -lpthread -L/usr/lib64/boost${BOOST_RELEASE} -lboost_filesystem -lboost_iostreams \
        -lboost_program_options -lboost_system ../ts_cRIOcpp/lib/libcRIOcpp.a \
	${SAL_WORK_DIR}/lib/libSAL_MTVMS.a \
        -L${LSST_SAL_PREFIX}/lib -L${SAL_WORK_DIR}/lib -lcurl -lrdkafka++ -lrdkafka -lavrocpp -lavro -ljansson -lserdes++ -lserdes -lsasl2
  LIBS_FLAGS += -L${LSST_SAL_PREFIX}/lib
else
  SAL_CPPFLAGS += $(PKG_CPPFLAGS) \
        -I${OSPL_HOME}/include -I${OSPL_HOME}/include/sys -I${OSPL_HOME}/include/dcps/C++/SACPP \
        -I${SAL_WORK_DIR}/MTVMS/cpp/src -I${SAL_WORK_DIR}/include \
        -I${SAL_HOME}/include -I${LSST_SDK_INSTALL}/include
endif

PKG_LIBS := \
        $(shell pkg-config yaml-cpp fftw3 --libs $(silence)) 

CPP := g++ -std=c++17 -Wall -fPIE ${c_opts}

# compile x86 simulator or real cRIO stuff
ifdef SIMULATOR
  C := gcc -Wall ${c_opts}
  CPP += -DSIMULATOR
  PKG_LIBS += $(shell pkg-config spdlog --libs $(silence))
else
  C := gcc -Wall -fmessage-length=0 ${c_opts}
  CPP += -fmessage-length=0
  PKG_LIBS += $(shell pkg-config spdlog --libs $(silence))
endif

ifdef WITH_SAL_KAFKA
  LIBS += $(PKG_LIBS) -ldl -lpthread -L/usr/lib64/boost${BOOST_RELEASE} -lboost_filesystem -lboost_iostreams \
	-lboost_program_options -lboost_system \
	-L${LSST_SAL_PREFIX}/lib -lcurl -lrdkafka++ -lrdkafka -lavrocpp -lavro -ljansson
else
  LIBS += $(PKG_LIBS) -ldl
endif

ifndef WITH_SAL_KAFKA
  SAL_LIBS := -ldcpssacpp -ldcpsgapi -lddsuser -lddskernel \
	-lpthread -lddsserialization -lddsconfparser -lddsconf -lddsdatabase \
	-lddsutil -lddsos \
	${SAL_WORK_DIR}/lib/libSAL_MTVMS.a 
  LIBS_FLAGS += -L${OSPL_HOME}/lib -L${LSST_SDK_INSTALL}/lib
endif

VERSION := $(shell git describe --tags --dirty 2>/dev/null || echo "unknown:non-git")
GIT_HASH := $(shell git rev-parse HEAD 2>/dev/null || echo "unknown-non-git")
