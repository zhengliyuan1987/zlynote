############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
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
