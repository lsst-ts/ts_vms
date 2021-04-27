include Makefile.inc

.PHONY: all clean deploy deploy_target tests FORCE doc simulator

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
	@$(foreach file,ts_MTVMS src/ts_MTVMS.cpp.o doc, echo '[RM ] ${file}'; $(RM) -r $(file);)
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
