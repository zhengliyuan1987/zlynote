open_project -reset uram_array.prj
set_top dut
add_files dut.cpp -cflags "-I. -I../../include"
add_files -tb test.cpp -cflags "-I. -I../../include"
open_solution "solution1"
set_part {xcvu9p-fsgd2104-2-i}
create_clock -period 3.33 -name default
set_clock_uncertainty 27%
csim_design
csynth_design
cosim_design -trace_level all
#config_export -format ip_catalog -rtl verilog
#export_design -rtl verilog -format ip_catalog
exit
