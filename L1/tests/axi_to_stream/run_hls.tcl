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

source settings.tcl

set PROJ "test.prj"
set SOLN "solution1"
set CLKP 2.5

open_project -reset $PROJ

add_files test_axi_to_stream.cpp -cflags "-I${XF_PROJ_ROOT}/L1/include"
add_files -tb test_axi_to_stream.cpp -cflags "-I${XF_PROJ_ROOT}/L1/include"
add_files -tb l_discount_fix.bin
add_files -tb l_orderkey_nfdint.bin
add_files -tb l_orderkey_unaligned.bin
add_files -tb l_discount_fix.txt
add_files -tb l_orderkey_nfdint.txt
add_files -tb l_orderkey_unaligned.txt
add_files -tb l_discount_few0.bin
add_files -tb l_discount_few0.txt
add_files -tb l_discount_few.txt
add_files -tb l_discount_few.bin

# test the top_align_axi_to_stream
set_top top_align_axi_to_stream
#
##corner test, modify define in testbanch##
#set host_args "-dataFile l_discount_few0.bin    -isALBIN 1 -isALREF 1 -isZERO 0"

##functional test##
set host_args "-dataFile l_discount_fix.bin    -isALBIN 1 -isALREF 1"
#set host_args "-dataFile l_orderkey_nfdint.bin -isALBIN 1 -isALREF 0"

# test the top_general_axi_to_stream
#set_top top_general_axi_to_stream
##corner test, modify define set testbanch##
#set host_args "-dataFile l_discount_few.bin       -isALBIN 0 -isALREF 1 -isZERO 0"
#
##functional test##
#set host_args "-dataFile l_discount_fix.bin       -isALBIN 0 -isALREF 1 -isZERO 1"
#set host_args "-dataFile l_orderkey_nfdint.bin    -isALBIN 0 -isALREF 0 -isZERO 1"
#set host_args "-dataFile l_orderkey_unaligned.bin -isALBIN 0 -isALREF 1 -isZERO 0"

open_solution -reset $SOLN

set_part $XPART
create_clock -period $CLKP

if {$CSIM == 1} {
  csim_design -compiler gcc -argv "$host_args"
 # csim_design -compiler gcc
}

if {$CSYNTH == 1} {
  csynth_design
}

if {$COSIM == 1} {
  cosim_design -compiler gcc -argv "$host_args"
 # cosim_design
}

if {$VIVADO_SYN == 1} {
  export_design -flow syn -rtl verilog
}

if {$VIVADO_IMPL == 1} {
  export_design -flow impl -rtl verilog
}

if {$QOR_CHECK == 1} {
  puts "QoR check not implemented yet"
}

exit
