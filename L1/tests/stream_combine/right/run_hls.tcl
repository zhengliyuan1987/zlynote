open_project -reset "stream_combine_right.prj"

add_files "test_right.cpp" -cflags "-I ../../../include"
add_files -tb "test_right.cpp" -cflags "-I ../../../include"
set_top dut

open_solution -reset "solutionright"
set_part virtexuplus
create_clock -period 3

csim_design -compiler gcc

csynth_design

cosim_design

exit
