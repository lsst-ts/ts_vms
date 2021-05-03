include Makefile.inc

.PHONY: all clean deploy deploy_target tests FORCE doc simulator ipk

# Add inputs and outputs from these tool invocations to the build variables 
#

# All Target
all: ts_MTVMS

src/libVMS.a: FORCE
	$(MAKE) -C src libVMS.a

# Tool invocations
ts_MTVMS: src/ts_MTVMS.cpp.o src/libVMS.a
	@echo '[LD ] $@'
	${co}$(CPP) $(LIBS_FLAGS) -o $@ $^ $(LIBS)

# Other Targets
clean:
	@$(foreach file,ts_MTVMS src/ts_MTVMS.cpp.o doc build *.ipk, echo '[RM ] ${file}'; $(RM) -r $(file);)
	@$(foreach dir,src tests,$(MAKE) -C ${dir} $@;)

# file targets
src/%.cpp.o: src/%.cpp
	$(MAKE) -C src $(patsubst src/%,%,$@)

CRIO_IP:=139.229.178.183 139.229.178.193

deploy_target: ts_MTVMS
	@echo 'Installing ${cip}'
	@echo '[SCP] $^'
	${co}scp $^ admin@${cip}:
	${co}ssh admin@${cip} 'mkdir -p Bitfiles'
	@echo '[SCP] Bitfiles/NiFpga_VMS_3_Master.lvbitx'
	${co}scp "Bitfiles/NiFpga_VMS_3_Master.lvbitx" admin@${cip}:Bitfiles/
	@echo '[SCP] Bitfiles/NiFpga_VMS_6_Master.lvbitx'
	${co}scp "Bitfiles/NiFpga_VMS_6_Master.lvbitx" admin@${cip}:Bitfiles/
	@echo '[SCP] Bitfiles/NiFpga_VMS_3_Slave.lvbitx'
	${co}scp "Bitfiles/NiFpga_VMS_3_Slave.lvbitx" admin@${cip}:Bitfiles/
	@echo '[SCP] Bitfiles/NiFpga_VMS_6_Slave.lvbitx'
	${co}scp "Bitfiles/NiFpga_VMS_6_Slave.lvbitx" admin@${cip}:Bitfiles/
	@echo '[SCP] Bitfiles/NiFpga_VMS_CameraRotator.lvbitx'
	${co}scp "Bitfiles/NiFpga_VMS_CameraRotator.lvbitx" admin@${cip}:Bitfiles/

deploy: ts_MTVMS
	@$(foreach cip,${CRIO_IP},${MAKE} cip=${cip} deploy_target;)

tests: tests/Makefile tests/*.cpp
	@${MAKE} -C tests

run_tests: tests
	@${MAKE} -C tests run

junit: tests
	@${MAKE} -C tests junit

doc:
	${co}doxygen Doxyfile

simulator:
	@${MAKE} SIMULATOR=1

ipk: ts_MTVMS tsvmsd_$(VERSION)_x86.ipk

tsvmsd_$(VERSION)_x86.ipk:
	@echo '[MK ] ipk'
	${co}mkdir -p ipk/data/usr/sbin
	${co}mkdir -p ipk/data/etc/init.d
	${co}mkdir -p ipk/data/etc/default
	${co}mkdir -p ipk/control
	${co}cp ts_MTVMS ipk/data/usr/sbin/tsvmsd
	${co}cp init ipk/data/etc/init.d/tsvmsd
	${co}cp default_tsvmsd ipk/data/etc/default/tsvmsd
	${co}sed s?@VERSION@?$(VERSION)?g control.ipk.in > ipk/control/control
	${co}echo "2.0" > ipk/debian-binary
	${co}tar czf ipk/data.tar.gz -P --transform "s#^ipk/data#.#" --owner=0 --group=0 ipk/data
	${co}tar czf ipk/control.tar.gz -P --transform "s#^ipk/control#.#" --owner=0 --group=0 ipk/control
	${co}ar r $@ ipk/control.tar.gz ipk/data.tar.gz ipk/debian-binary
