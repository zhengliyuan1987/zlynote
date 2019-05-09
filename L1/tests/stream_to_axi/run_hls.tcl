open_project -reset stream_to_axi.prj
set_top stream_to_axi_test
add_files test.cpp -cflags "-I../../include"
add_files -tb test.cpp -cflags "-I../../include"
open_solution "solution1"
set_part {xcvu9p-fsgd2104-2-i} -tool vivado
create_clock -period 3.33 -name default
set_clock_uncertainty 0.9
#source "./stream_to_axi.prj/solution1/directives.tcl"
csim_design
csynth_design
cosim_design -trace_level all
#export_design -format ip_catalog
exit
