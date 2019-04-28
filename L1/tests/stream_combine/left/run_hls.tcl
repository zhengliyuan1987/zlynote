open_project -reset "stream_combine_left.prj"

add_files "test_left.cpp" -cflags "-I ../../../include"
add_files -tb "test_left.cpp" -cflags "-I ../../../include"
set_top dut

open_solution -reset "solutionleft"
set_part virtexuplus
create_clock -period 3

csim_design -compiler gcc

csynth_design

cosim_design

exit
