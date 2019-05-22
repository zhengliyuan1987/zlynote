
open_project -reset "test.prj"

#add_files code.cpp -cflags "-I../../../include/"  
#add_files -tb code.cpp -cflags "-I../../../include/"
add_files code.cpp -cflags "-I../../include/"  
add_files -tb code.cpp -cflags "-I../../include/"

set_top  round_robin_mpu

open_solution -reset "solution1"
set_part virtexuplus
create_clock -period 2.5

csim_design -compiler gcc

csynth_design

#cosim_design
cosim_design -wave_debug -tool xsim -trace_level all

exit
