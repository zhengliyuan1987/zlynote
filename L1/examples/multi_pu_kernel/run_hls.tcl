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
open_project -reset  axi_stream_mpu.prj
config_debug

add_files test_axi_to_stream.cpp  -cflags "-std=gnu++0x -g -I ../../include"
add_files -tb test_axi_to_stream.cpp -cflags "-std=gnu++0x -g -I ../../include"

set_top top_core
set host_args ""

open_solution -reset "solution1"
set_part virtexuplus
create_clock -period 2.5 -name default
set_clock_uncertainty 27.000000%

csim_design -compiler gcc -argv "$host_args"

csynth_design

cosim_design -wave_debug -tool xsim -trace_level all

#cosim_design -compiler gcc -argv "$host_args"

#export_design -flow impl -rtl verilog -format ip_catalog

exit
