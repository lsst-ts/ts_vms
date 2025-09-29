FROM lsstts/salobj:develop as crio-develop

USER root
RUN chmod a+rwX -R /home/saluser/
USER saluser

ARG XML_BRANCH=develop

WORKDIR /home/saluser

RUN source ~/.setup.sh \
    && mamba install -y readline yaml-cpp boost-cpp catch2 spdlog fmt fftw \
    && echo > .crio_setup.sh -e \
echo "Configuring cRIO development environment" \\n\
export SHELL=bash \\n\
source /home/saluser/repos/ts_sal/setupKafka.env \\n\
export PATH=\$CONDA_PREFIX/bin:\$PATH \\n\
export LIBS="-L\$CONDA_PREFIX/lib" \\n\
export CPP_FLAGS="-I\$CONDA_PREFIX/include" \\n

# temporary upgrade ts_sal, needed for current develop ts_xml (which changed paths)
RUN source ~/.crio_setup.sh && cd repos/ts_sal \
    && git fetch && git checkout develop && git pull

RUN source ~/.crio_setup.sh && cd $TS_XML_DIR \
    && git fetch && git checkout $XML_BRANCH && git pull \
    && salgeneratorKafka generate cpp MTVMS

FROM crio-develop

ARG cRIO_CPP=develop
ARG VMS=develop
ARG TARGET=simulator

RUN cd repos && git clone --branch $cRIO_CPP https://github.com/lsst-ts/ts_cRIOcpp
RUN source ~/.crio_setup.sh && cd repos/ts_cRIOcpp && make

RUN cd repos && git clone --branch $VMS https://github.com/lsst-ts/ts_vms
RUN source ~/.crio_setup.sh && cd repos/ts_vms && make $TARGET

SHELL ["/bin/bash", "-lc"]
