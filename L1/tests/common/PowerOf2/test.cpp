#include <iostream>
#include "xf_utils_hw/common.hpp"

int dut() {
    static const int a = 5;
    int c = xf::common::utils_hw::PowerOf2<a>::value;
    return c;
}

int main(int argc, const char* argv[]) {
    int c = dut();
    if (c != 32) {
        std::cout << "FAIL" << std::endl;
        return 1;
    }
    std::cout << "PASS" << std::endl;
    return 0;
}
