############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project prj_axi_to_stream
config_debug

set_top top_align_axi_to_stream
add_files test_axi_to_stream.cpp  -cflags "-std=gnu++0x -g -I ../../include"
add_files -tb l_discount_fix.bin
add_files -tb l_orderkey_nfdint.bin
add_files -tb l_discount_fix.txt
add_files -tb l_orderkey_nfdint.txt
add_files -tb test_axi_to_stream.cpp -cflags "-std=gnu++0x -g -I ../../include"

set host_args "-dataFile l_discount_fix.bin -alignedRef 1"
#set host_args "-dataFile l_orderkey_nfdint.bin -alignedRef 0"

open_solution -reset "solution1"
set_part virtexuplus
create_clock -period 3.33 -name default
set_clock_uncertainty 1.05

csim_design -compiler gcc -argv "$host_args"

csynth_design

cosim_design -compiler gcc -argv "$host_args"

#export_design -flow impl -rtl verilog -format ip_catalog

exit
