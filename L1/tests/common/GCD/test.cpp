#include <iostream>
#include "xf_utils_hw/common.hpp"

int dut() {
    static const int a = 99;
    static const int b = 77;
    int c = xf::common::utils_hw::GCD<a, b>::value;
    return c;
}

int main(int argc, const char* argv[]) {
    int c = dut();
    if (c != 11) {
        std::cout << "FAIL" << std::endl;
        return 1;
    }
    std::cout << "PASS" << std::endl;
    return 0;
}
