#
# Copyright 2020 Xilinx, Inc.
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

############################## Help Section ##############################
.PHONY: help

help::
	$(ECHO) "Makefile Usage:"
	$(ECHO) "  make all HOST_ARCH=<aarch32/aarch64/x86>"
	$(ECHO) "      Command to compile test binary."
	$(ECHO) ""
	$(ECHO) "  make clean"
	$(ECHO) "      Command to remove the generated non-hardware files."
	$(ECHO) ""

############################## Setting up Project Variables ##############################
MK_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
XF_PROJ_ROOT ?= $(shell bash -c 'export MK_PATH=$(MK_PATH); echo $${MK_PATH%L3/tests/arg_parser/*}')
CUR_DIR := $(patsubst %/,%,$(dir $(MK_PATH)))
XFLIB_DIR = $(XF_PROJ_ROOT)

HOST_ARCH ?= x86

# Set to make utils.mk happy
TARGET = sw_emu
include ./utils.mk

BUILD_DIR := build_dir

############################## Setting up Host Variables ##############################
#Include Required Host Source Files
HOST_SRCS += $(CUR_DIR)/src/test.cpp
CXXFLAGS += -I$(XFLIB_DIR)/L3/include

# Host compiler global settings
CXXFLAGS += -I$(XILINX_VIVADO)/include -std=c++14 -O3 -Wall -Wno-unknown-pragmas -Wno-unused-label
CXXFLAGS += -I$(CUR_DIR)/src/

EXE_NAME := test.exe
EXE_FILE := $(BUILD_DIR)/$(EXE_NAME)
HOST_ARGS := --xclbin vadd.xclbin -q 0 -v -i 2

ifneq ($(HOST_ARCH), x86)
	LDFLAGS += --sysroot=$(SYSROOT)
endif

############################## Setting Targets ##############################

.PHONY: all clean run
all: host

.PHONY: host
host: $(EXE_FILE)

############################## Setting Rules for Host (Building Host Executable) ##############################
$(EXE_FILE): $(HOST_SRCS)
	mkdir -p $(BUILD_DIR)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

############################## Setting Essential Checks and Running Rules ##############################
run: all
	$(EXE_FILE) $(HOST_ARGS)

############################## Cleaning Rules ##############################
clean:
	-$(RMDIR) $(BUILD_DIR)
