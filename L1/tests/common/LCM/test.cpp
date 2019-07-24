#include <iostream>
#include "xf_utils_hw/common.hpp"

int dut() {
    static const int a = 15;
    static const int b = 35;
    int c = xf::common::utils_hw::LCM<a, b>::value;
    return c;
}

int main(int argc, const char* argv[]) {
    int c = dut();
    if (c != 105) {
        std::cout << "FAIL" << std::endl;
        return 1;
    }
    std::cout << "PASS" << std::endl;
    return 0;
}
