############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project -reset prj_axi_to_multi_stream
config_debug

add_files test_axi_to_multi_stream.cpp  -cflags "-std=gnu++0x -g -I ../../include"
add_files -tb l_orderkey_xilinx.bin
#add_files -tb l_orderkey_veint.bin
#add_files -tb l_orderkey_500k.bin
add_files -tb test_axi_to_multi_stream.cpp -cflags "-std=gnu++0x -g -I ../../include"


#set_top top_axi_to_multi_stream
set_top top_for_co_sim

set host_args "-dataFile l_orderkey_xilinx.bin "
#set host_args "-dataFile l_orderkey_500k.bin "

open_solution -reset "solution1"
#open_solution -reset "sol_axi512"
set_part virtexuplus
create_clock -period 3.33 -name default
set_clock_uncertainty 27.000000%

csim_design -compiler gcc -argv "$host_args"

csynth_design

cosim_design -compiler gcc -argv "$host_args"
#cosim_design -wave_debug -trace_level all -argv "$host_args" -tool xsim

export_design -flow impl -rtl verilog -format ip_catalog

exit
