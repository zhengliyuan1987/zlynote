############################################################
## This file is generated automatically by Vivado HLS.
## Please DO NOT edit it.
## Copyright (C) 1986-2018 Xilinx, Inc. All Rights Reserved.
############################################################
open_project uram_array.prj
set_top core_test
add_files test.cpp -cflags "-I../../include"
add_files -tb test.cpp -cflags "-I../../include"
open_solution "solution1"
set_part {xcvu9p-fsgd2104-2-i} -tool vivado
create_clock -period 3.33 -name default
config_export -format ip_catalog -rtl verilog
set_clock_uncertainty 0.9
#source "./access_to_uram.prj/solution1/directives.tcl"
csim_design -O
csynth_design
cosim_design -trace_level all
#export_design -rtl verilog -format ip_catalog
exit
