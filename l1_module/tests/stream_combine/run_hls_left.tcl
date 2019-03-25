
open_project -reset "stream_combine.prj"

add_files "test_left.cpp" -cflags "-I ../../include"
add_files -tb "test_left.cpp" -cflags "-I ../../include"
set_top dut

open_solution -reset "solution1"
set_part virtexuplus
create_clock -period 2.5

csim_design -compiler gcc

csynth_design
