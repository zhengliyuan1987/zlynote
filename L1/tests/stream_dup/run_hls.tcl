open_project -reset "stream_dup.prj"

add_files "test.cpp" -cflags "-I ../../include"
add_files -tb "test.cpp" -cflags "-I ../../include"

open_solution -reset "dut0"
set_top dut0
set_part virtexuplus
create_clock -period 2.5

csim_design -compiler gcc -argv "0"
csynth_design
cosim_design -argv "0"

open_solution -reset "dut1"
set_top dut1
set_part virtexuplus
create_clock -period 2.5

csim_design -compiler gcc -argv "1"
csynth_design
cosim_design -argv "1"

exit
