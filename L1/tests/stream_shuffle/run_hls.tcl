
open_project -reset "stream_shuffle.prj"

add_files test.cpp -cflags "-I../../include/"  
add_files -tb test.cpp -cflags "-I../../include/"

set_top  top

open_solution -reset "solution1"
set_part virtexuplus

create_clock -period 2.5

csim_design -compiler gcc

csynth_design


#cosim_design -wave_debug -tool xsim -trace_level all

#cosim_design 


#export_design -flow impl -rtl verilog -format ip_catalog

exit