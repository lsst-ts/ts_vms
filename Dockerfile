FROM lsstts/develop-env:develop AS crio-develop

USER root

RUN rm -rf  /home/saluser/repos/ts_sal/include/avro/

RUN cd /opt/lsst/tssw/ && git clone https://github.com/apache/avro 

RUN source /home/saluser/.setup_salobj.sh && cd /opt/lsst/tssw/avro/lang/c \
    && cmake . && make && make install \
    && cd ../c++ && cmake . && make && make install

RUN cd /opt/lsst/tssw/ && git clone https://github.com/confluentinc/libserdes 

COPY libserdes.patch /opt/lsst/tssw/libserdes
RUN source /home/saluser/.setup_salobj.sh && cd /opt/lsst/tssw/libserdes \
    && patch -p1 < libserdes.patch

RUN source /home/saluser/.setup_salobj.sh \
    && export CPATH="/usr/local/include" \
    && export LIBRARY_PATH="/usr/local/lib64:/opt/lsst/tssw/ts_sal/lib" \
    && cd /opt/lsst/tssw/libserdes && ./configure && make && make install

USER saluser
ARG XML_BRANCH=develop
WORKDIR /home/saluser

RUN source ~/.setup_salobj.sh \
    && mamba install -y readline yaml-cpp catch2 spdlog texlive-core fftw ghostscript 

RUN source ~/.setup_salobj.sh \
    && echo > .crio_setup.sh -e \
echo "Configuring cRIO development environment" \\n\
export SAL_HOME=/opt/lsst/tssw/ts_sal/lsstsal\\n\
export SAL_WORK_DIR=/home/saluser/repos/ts_sal/test\\n\
export LSST_KAFKA_PREFIX=sal\\n\
export LSST_KAFKA_SECURITY_MECHANISM=SCRAM-SHA-512\\n\
export LSST_KAFKA_CLASSDIR=/opt/lsst/tssw/ts_sal/lib\\n\
export LSST_KAFKA_HOST=broker\\n\
export LSST_KAFKA_IP=\\n\
export LSST_KAFKA_LOCAL_SCHEMAS=/opt/lsst/tssw/ts_sal/test\\n\
export LSST_KAFKA_BROKER_PORT=29092\\n\
export LSST_KAFKA_BROKER_ADDR=broker:29092\\n\
export LSST_KAFKA_SECURITY_PROTOCOL=SASL_SSL\\n\
export AVRO_RELEASE=1.11.3\\n\
export LSST_TOPIC_SUBNAME=sal\\n\
\\n\
export LSST_KAFKA_BROKER_ADDR="broker:29092"\\n\
export LSST_SCHEMA_REGISTRY_URL="http://schema-registry:8081"\\n\
\\n\
export LSST_SAL_PREFIX=/usr/local\\n\
\\n\
export LSST_KAFKA_HOST=${LSST_KAFKA_BROKER_ADDR%:*}\\n\
export LSST_KAFKA_BROKER_PORT=${LSST_KAFKA_BROKER_ADDR##*:}\\n\
export LSST_KAFKA_PREFIX=sal\\n\
export LSST_KAFKA_LOCAL_SCHEMAS=$SAL_WORK_DIR\\n\
\\n\
export TS_CONFIG_OCS_DIR=/lsst/ts_config_ocs/\\n\
\\n\
export PKG_CONFIG_PATH=/usr/local/lib64/pkgconfig\\n\
\\n\
export LIBRARY_PATH=/usr/local/lib64:/usr/local/lib:/opt/lsst/tssw/ts_sal/lib\\n\
\\n\
source /home/saluser/.setup_salobj.sh \
source /opt/lsst/tssw/ts_sal/setupKafka.env \
\\n\
setup ts_idl -t current \\n\
setup ts_sal -t current \\n\
setup ts_salobj -t current \\n\
setup ts_xml -t current \\n\
\\n\
export CPATH=\"/usr/local/include\"\\n\
\\n\
export SAL_DIR=/opt/lsst/tssw/ts_sal/lsstsal/scripts

RUN source ~/.crio_setup.sh \
    && cd $TS_XML_DIR \
    && git fetch && git checkout $XML_BRANCH && git pull \
    && pip install .

RUN source ~/.crio_setup.sh \
    && salgeneratorKafka generate cpp MTVMS

ARG cRIO_CPP=v1.16.0
ARG VMS=develop
ARG TARGET=simulator

RUN source ~/.crio_setup.sh \
    && git clone --branch $cRIO_CPP https://github.com/lsst-ts/ts_cRIOcpp \
    && cd ts_cRIOcpp && make

RUN source ~/.crio_setup.sh \
    && git clone --branch $VMS https://github.com/lsst-ts/ts_vms \
    && cd ts_vms && make $TARGET

SHELL ["/bin/bash", "-lc"]
