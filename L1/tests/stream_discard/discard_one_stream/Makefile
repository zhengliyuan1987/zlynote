#
# Copyright 2018 Xilinx, Inc.
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

#

.PHONY: help

help::
	@echo ""
	@echo "Makefile Usage:"
	@echo ""
	@echo "  make check DEVICE=<FPGA platform> CSIM=1 CSYNTH=1 COSIM=1"
	@echo "      Command to synthesis and run the test for specified device."
	@echo ""
	@echo "      DEVICE is case-insensitive and support awk regex."
	@echo "      For example, \`make check TARGET=hw DEVICE='u200.*qdma'\`"
	@echo "      It can also be an absolute path to platform file."
	@echo ""
	@echo "  make check XPART=<FPGA part name>"
	@echo "      Command to synthesis and run the test for specified part."
	@echo "      When XPART is set, DEVICE will be ignored."
	@echo ""
	@echo "  make clean "
	@echo "      Command to remove the generated files."
	@echo ""

# -----------------------------------------------------------------------------
#                environment validation and general setup
ifndef XILINX_VIVADO
$(error XILINX_VIVADO should have been set by setting64.sh from SDx install path.)
endif

ifeq (,$(XPART))

# try to figure out from DEVICE
DEVICE ?= u200
ifneq ("$(wildcard $(DEVICE))","")
# Use DEVICE as a file path
XPLATFORM := $(DEVICE)
else
# Use DEVICE as a file name pattern
ifndef XILINX_SDX
$(error XILINX_SDX should have been set by setting64.sh from SDx install path.)
endif
ifndef PLATFORM_REPO_PATHS
$(error PLATFORM_REPO_PATHS should have be defined when using DEVICE as a pattern.)
endif
DEVICE_L := $(shell echo $(DEVICE) | tr A-Z a-z)
XPLATFORM_PATHS := $(subst :, ,$(PLATFORM_REPO_PATHS)) $(XILINX_SDX)/platforms
XPLATFORMS := $(foreach p, $(XPLATFORM_PATHS), $(wildcard $(p)/*/*.xpfm))
XPLATFORM := $(strip $(foreach p, $(XPLATFORMS), $(shell echo $(p) | awk '$$1 ~ /$(DEVICE_L)/')))
ifeq (,$(XPLATFORM))
$(error No platform matched, available platforms: $(XPLATFORMS))
endif
ifneq (,$(word 2,$(XPLATFORM)))
$(error More than one platform matched: $(XPLATFORM))
endif
endif

XPART = $(shell platforminfo --json="hardwarePlatform.board.part" --platform $(XPLATFORM))
$(info Part in device $(XPLATFORM) is $(XPART))
endif

# Path setup
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))

.PHONY: check run setup runhls clean

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

check: setup runhls

run: check

CASE_ROOT ?= $(CUR_DIR)

setup:
	@echo 'set XPART $(XPART)' > ./settings.tcl
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

