
open_project -reset "test_comb_lsb.prj"

add_files test.cpp -cflags "-I../../../include/"  
add_files -tb test.cpp -cflags "-I../../../include/"

set_top  test_core_comb_lsb

open_solution -reset "solution1"
set_part virtexuplus
create_clock -period 2.5

csim_design -compiler gcc

csynth_design

cosim_design

exit
