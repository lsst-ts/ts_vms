FROM ts-dockerhub.lsst.org/deploy-crio:c0045 AS crio-develop

USER saluser
ARG XML_BRANCH=develop
WORKDIR /home/saluser

ARG KAFKA_HOST=
ARG KAFKA_BROKER_PORT=
ARG SCHEMA_REGISTRY_URI=

RUN [ -z $KAFKA_HOST -o -z $KAFKA_BROKER_PORT ] || echo >> .crio_setup.sh -e \
\\n\
export LSST_KAFKA_BROKER_ADDR="${KAFKA_HOST}:${KAFKA_BROKER_PORT}"\\n\
export LSST_KAFKA_HOST=${KAFKA_HOST}\\n\
export LSST_KAFKA_BROKER_PORT=${KAFKA_BROKER_PORT}

RUN [ -z $SCHEMA_REGISTRY_URI ] || echo >> .crio_setup.sh -e \
\\n\
export LSST_SCHEMA_REGISTRY_URL=${SCHEMA_REGISTRY_URI}

RUN source ~/.crio_setup.sh && mamba install -y fftw

RUN source ~/.crio_setup.sh && cd $TS_XML_DIR \
    && git fetch && git checkout $XML_BRANCH && git pull \
    && pip install .

RUN source ~/.crio_setup.sh \
    && MAKEFLAGS="-j$(nproc)" LIBSCHEMAREGISTRY_VCPKG_LIB=/opt/vcpkg/installed/x64-linux/lib salgeneratorKafka generate cpp MTVMS

ARG cRIO_CPP=v1.16.1
ARG VMS=develop
ARG TARGET=simulator

RUN source ~/.crio_setup.sh  \
    && cd ts_cRIOcpp && git fetch && git checkout $cRIO_CPP \
    && make clean && make -j$(nproc)

RUN source ~/.crio_setup.sh \
    && git clone --branch $VMS https://github.com/lsst-ts/ts_vms \
    && cd ts_vms \
    && PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/opt/lsst/software/stack/miniconda/share/pkgconfig LIBSCHEMAREGISTRY_VCPKG_LIB=/opt/vcpkg/installed/x64-linux/lib make -j$(nproc) SIMULATOR=1 $TARGET

SHELL ["/bin/bash", "-lc"]
