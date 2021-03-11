include Makefile.inc

.PHONY: all clean deploy tests FORCE doc

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
	@$(foreach file,ts_VMS src/ts_VMS.cpp.o doc, echo '[RM ] ${file}'; $(RM) -r $(file);)
	@$(foreach dir,src tests,$(MAKE) -C ${dir} $@;)

# file targets
src/%.cpp.o: src/%.cpp
	$(MAKE) -C src $(patsubst src/%,%,$@)

CRIO_IP = 139.229.178.4

deploy: ts_MTVMS
	@echo '[SCP] $^'
	${co}scp $^ admin@${CRIO_IP}:
	@echo '[SCP] FPGA/FPGA\ Bitfiles/VMS_6_Master.lvbitx'
	${co}scp "FPGA/FPGA Bitfiles/VMS_6_Master.lvbitx" admin@${CRIO_IP}:Bitfiles

tests: tests/Makefile tests/*.cpp
	@${MAKE} -C tests

run_tests: tests
	@${MAKE} -C tests run

junit: tests
	@${MAKE} -C tests junit

doc:
	${co}doxygen Doxyfile
