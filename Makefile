include Makefile.inc

.PHONY: all clean tests FORCE doc simulator ipk

# Add inputs and outputs from these tool invocations to the build variables 
#

# All Target
all: vmscli utils/vmsrawdump ts-VMSd

src/libVMS.a: FORCE
	$(MAKE) -C src libVMS.a

# Tool invocations
ts-VMSd: src/ts-VMSd.cpp.o src/libVMS.a
	@echo '[LD ] $@'
	${co}$(CPP) $(LIBS_FLAGS) -o $@ $^ ${CRIOCPP}/lib/libcRIOcpp.a $(LIBS) $(shell pkg-config --libs readline $(silence)) -lreadline

vmscli: src/vmscli.cpp.o src/libVMS.a
	@echo '[LD ] $@'
	${co}$(CPP) $(LIBS_FLAGS) -o $@ $^  ../ts_cRIOcpp/lib/libcRIOcpp.a $(LIBS) $(shell pkg-config --libs readline $(silence)) -lreadline

utils/vmsrawdump: utils/vmsrawdump.cpp
	@${MAKE} -C utils

# Other Targets
clean:
	@$(foreach file,ts-VMSd vmscli src/ts-VMS.cpp.o src/vmscli.cpp.o *.ipk ipk, echo '[RM ] ${file}'; $(RM) -r $(file);)
	@$(foreach dir,src tests,$(MAKE) -C ${dir} $@;)

# file targets
src/%.cpp.o: src/%.cpp
	$(MAKE) -C src $(patsubst src/%,%,$@)

tests: tests/Makefile tests/*.cpp
	@${MAKE} -C tests SIMULATOR=1

run_tests: tests
	@${MAKE} -C tests run

junit: tests
	@${MAKE} -C tests junit SIMULATOR=1

doc:
	${co}doxygen Doxyfile

simulator:
	@${MAKE} SIMULATOR=1 DEBUG=1

ipk: ts-VMS_$(VERSION)_x64.ipk

TS_DDSCONFIG=../ts_ddsconfig

ts-VMS_$(VERSION)_x64.ipk: vmscli utils/vmsrawdump ts-VMSd
	@echo '[MK ] ipk $@'
	${co}mkdir -p ipk/data/usr/bin
	${co}mkdir -p ipk/data/usr/sbin
	${co}mkdir -p ipk/data/etc/init.d
	${co}mkdir -p ipk/data/etc/default
	${co}mkdir -p ipk/data/var/lib/MTVMS
	${co}mkdir -p ipk/control
	${co}cp vmscli ipk/data/usr/bin/
	${co}cp utils/vmsrawdump ipk/data/usr/bin/
	${co}cp ts-VMSd ipk/data/usr/sbin/
	${co}cp init ipk/data/etc/init.d/ts-VMS
	${co}cp default_ts-VMS ipk/data/etc/default/ts-VMS
	${co}cp -r SettingFiles/* ipk/data/var/lib/MTVMS
	${co}cp -r Bitfiles/* ipk/data/var/lib/MTVMS
	${co}cp -r ${TS_DDSCONFIG}/python/lsst/ts/ddsconfig/data/config/ospl-embedded-shmem.xml ipk/data/var/lib/MTVMS || echo "Cannot find ${TS_DDSCONFIG} ospl-embedded-shmem.xml - check it out?"
	${co}cp -r ${TS_DDSCONFIG}/python/lsst/ts/ddsconfig/data/qos/QoS.xml ipk/data/var/lib/MTVMS || echo "Cannot find ${TS_DDSCONFIG} QoS.xml - check it out?"
	${co}sed s?@VERSION@?$(VERSION)?g control.ipk.in > ipk/control/control
	${co}cp postinst prerm postrm ipk/control
	${co}echo -e "/etc/default/ts-VMS\n/var/lib/MTVMS/M1M3.yaml\n/var/lib/MTVMS/M2.yaml\n/var/lib/MTVMS/CameraRotator.yaml\n/var/lib/MTVMS/ospl-embedded-shmem.xml\n/var/lib/MTVMS/QoS.xml" > ipk/control/conffiles
	${co}echo "2.0" > ipk/debian-binary
	${co}tar czf ipk/data.tar.gz -P --transform "s#^ipk/data#.#" --owner=0 --group=0 ipk/data
	${co}tar czf ipk/control.tar.gz -P --transform "s#^ipk/control#.#" --owner=0 --group=0 ipk/control
	${co}ar r $@ ipk/control.tar.gz ipk/data.tar.gz ipk/debian-binary
