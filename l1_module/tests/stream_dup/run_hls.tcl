
open_project -reset "stream_dup.prj"

add_files "test.cpp" -cflags "-I ../../include"
add_files -tb "test.cpp" -cflags "-I ../../include"
set_top dut

open_solution -reset "solution1"
set_part virtexuplus
create_clock -period 3

csim_design -compiler gcc

csynth_design

cosim_design

exit
