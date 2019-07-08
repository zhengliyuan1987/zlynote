#
# Copyright 2019 Xilinx, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
CASE_ROOT ?= $(CUR_DIR)

# MK_INC_BEGIN hls_common.mk

.PHONY: help

help::
	@echo ""
	@echo "Makefile Usage:"
	@echo ""
	@echo "  make run DEVICE=<FPGA platform> CSIM=1 CSYNTH=1 COSIM=1"
	@echo "      Command to run the selected tasks for specified device."
	@echo ""
	@echo "      Valid tasks are CSIM, CSYNTH, COSIM, VIVADO_SYN, VIVADO_IMPL"
	@echo ""
	@echo "      DEVICE is case-insensitive and support awk regex."
	@echo "      For example, \`make run DEVICE='u200.*qdma' CSIM=1\`"
	@echo "      It can also be an absolute path to platform file."
	@echo ""
	@echo "  make run XPART=<FPGA part name> CSIM=1"
	@echo "      Alternatively, the FPGA part can be speficied via XPART."
	@echo "      For example, \`make run XPART='xcu200-fsgd2104-2-e' CSIM=1\`"
	@echo "      When XPART is set, DEVICE will be ignored."
	@echo ""
	@echo "  make clean "
	@echo "      Command to remove the generated files."
	@echo ""

# MK_INC_END hls_common.mk

# MK_INC_BEGIN vivado.mk

TOOL_VERSION ?= 2018.3

ifeq (,$(XILINX_VIVADO))
XILINX_SDX = /opt/xilinx/Vivado/$(TOOL_VERSION)
ifeq (,$(wildcard $(XILINX_SDX)/bin/vivado))
$(error "Cannot locate Vivado installation. Please set XILINX_VIVADO variable.")
endif
endif
export XILINX_VIVADO

export PATH := $(XILINX_VIVADO)/bin:$(PATH)

# MK_INC_END vivado.mk

DEVICE ?= u200
# MK_INC_BEGIN set_part.mk

ifeq (,$(XPART))
# MK_INC_BEGIN sdx.mk

TOOL_VERSION ?= 2018.3

ifeq (,$(XILINX_SDX))
XILINX_SDX = /opt/xilinx/SDx/$(TOOL_VERSION)
ifeq (,$(wildcard $(XILINX_SDX)/bin/xocc))
$(error "Cannot locate SDx installation. Please set XILINX_SDX variable.")
endif
endif
export XILINX_SDX

ifeq (,$(XILINX_XRT))
XILINX_XRT = /opt/xilinx/xrt
ifeq (,$(wildcard $(XILINX_SDX)/lib/libxilinxopencl.so))
$(error "Cannot locate XRT installation. Please set XILINX_XRT variable.")
endif
endif
export XILINX_XRT

export PATH := $(XILINX_SDX)/bin:$(XILINX_XRT)/bin:$(PATH)

ifeq (,$(LD_LIBRARY_PATH))
export LD_LIBRARY_PATH := $(shell $(XILINX_SDX)/bin/ldlibpath.sh $(XILINX_SDX)/lib/lnx64.o):$(XILINX_XRT)/lib
else
export LD_LIBRARY_PATH := $(shell $(XILINX_SDX)/bin/ldlibpath.sh $(XILINX_SDX)/lib/lnx64.o):$(XILINX_XRT)/lib:$(LD_LIBRARY_PATH)
endif

# Target check
TARGET ?= sw_emu
ifeq ($(filter $(TARGET),sw_emu hw_emu hw),)
$(error TARGET is not sw_emu, hw_emu or hw)
endif

# MK_INC_END sdx.mk
# MK_INC_BEGIN set_platform.mk

ifneq (,$(wildcard $(DEVICE)))
# Use DEVICE as a file path
XPLATFORM := $(DEVICE)
else
# Use DEVICE as a file name pattern
# Default location of platforms.
ifndef PLATFORM_REPO_PATHS
ifneq (,$(wildcard /opt/xilinx/platforms/*))
PLATFORM_REPO_PATHS := /opt/xilinx/platforms
endif
endif
# Match the name
DEVICE_L := $(shell echo $(DEVICE) | tr A-Z a-z)
XPLATFORM_PATHS := $(subst :, ,$(PLATFORM_REPO_PATHS)) $(XILINX_SDX)/platforms
XPLATFORMS := $(foreach p, $(XPLATFORM_PATHS), $(wildcard $(p)/*/*.xpfm))
XPLATFORM := $(strip $(foreach p, $(XPLATFORMS), $(shell echo $(p) | awk '$$1 ~ /$(DEVICE_L)/')))
ifeq (,$(XPLATFORM))
define MSG_PLATFORM
No platform matched pattern '$(DEVICE)'.
Avaialble platforms are: $(XPLATFORMS)
To add more platforms, set the PLATFORM_REPO_PATHS variable.
endef
$(error $(MSG_PLATFORM))
endif
ifneq (,$(word 2,$(XPLATFORM)))
define MSG_DEVICE
More than one platform matched: $(XPLATFORM)
Please set DEVICE variable to specify a device. For example: DEVICE='u200.*xdma'
endef
$(error $(MSG_DEVICE))
endif
endif

XDEVICE := $(basename $(notdir $(XPLATFORM)))

# MK_INC_END set_platform.mk
# Query the part name of device
override XPART = $(shell PATH=$(PATH) platforminfo --json="hardwarePlatform.board.part" --platform $(XPLATFORM))
endif

# MK_INC_END set_part.mk

# MK_INC_BEGIN hls_test_rules.mk

.PHONY: run setup runhls clean

CSIM ?= 0
CSYNTH ?= 0
COSIM ?= 0
VIVADO_SYN ?= 0
VIVADO_IMPL ?= 0
QOR_CHECK ?= 0


# at least RTL synthesis before check QoR
ifeq (1,$(QOR_CHECK))
ifeq (0,$(VIVADO_IMPL))
VIVADO_SYN = 1
endif
CSYNTH = 1
endif

# at least synthesis before cosim
ifeq (1,$(COSIM))
CSYNTH = 1
endif

run: setup runhls

setup:
	@rm -f ./settings.tcl
	@if [ -n "$$CLKP" ]; then echo 'set CLKP $(CLKP)' >> ./settings.tcl ; fi
	@echo 'set XPART $(XPART)' >> ./settings.tcl
	@echo 'set CSIM $(CSIM)' >> ./settings.tcl
	@echo 'set CSYNTH $(CSYNTH)' >> ./settings.tcl
	@echo 'set COSIM $(COSIM)' >> ./settings.tcl
	@echo 'set VIVADO_SYN $(VIVADO_SYN)' >> ./settings.tcl
	@echo 'set VIVADO_IMPL $(VIVADO_IMPL)' >> ./settings.tcl
	@echo 'set QOR_CHECK $(QOR_CHECK)' >> ./settings.tcl
	@echo 'set CASE_ROOT "$(CASE_ROOT)"' >> ./settings.tcl
	@echo "Configured: settings.tcl"
	@echo "----"
	@cat ./settings.tcl
	@echo "----"

runhls: setup
	vivado_hls -f run_hls.tcl;

clean:
	rm -rf *.prj vivado_hls.log settings.tcl

.PHONY: check
check: run

# MK_INC_END hls_test_rules.mk
