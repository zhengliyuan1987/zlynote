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

open_project -reset "test.prj"

#add_files code.cpp -cflags "-I../../../include/"  
#add_files -tb code.cpp -cflags "-I../../../include/"
add_files code.cpp -cflags "-I../../include/"  
add_files -tb code.cpp -cflags "-I../../include/"

set_top  test_core

open_solution -reset "solution1"
set_part virtexuplus
create_clock -period 2.5

csim_design -compiler gcc

csynth_design

cosim_design
#cosim_design -wave_debug -tool xsim -trace_level all

exit
