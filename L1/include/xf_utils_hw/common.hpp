/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef XF_UTILS_HW_COMMON_H
#define XF_UTILS_HW_COMMON_H

/**
 * @file common.hpp
 * @brief Shared logic in utilities library.
 *
 * This file is part of XF Hardware Utilities Library.
 */

#include "xf_utils_hw/types.hpp"

namespace xf {
namespace common {
namespace utils_hw {

// Calculate Power of 2
template <int _N>
struct power_of_2 {
    static const unsigned value = (1u << _N);
};

// Greatest Common Divisor
template <int a, int b>
struct gcd {
    static const int value = gcd<b, a % b>::value;
};
template <int a>
struct gcd<a, 0> {
    static const int value = a;
};
// Least Common Multiple
template <int a, int b>
struct lcm {
    static const int value = a * b / gcd<b, a % b>::value;
};

template <int NStrm>
struct up_bound {
    // clang-format off
  static const int up= (NStrm >=128) ? 128 :
                        NStrm > 64   ? 128 :
                        NStrm > 32   ? 64  :
                        NStrm > 16   ? 32  :
                        NStrm > 8    ? 16  :
                        NStrm > 4    ? 8   :
                        NStrm > 2    ? 4   :
                        NStrm > 1    ? 2   : 1 ;
    // clang-format on
};

template <int N>
ap_uint<N> count_ones(ap_uint<N> y) {
#pragma HLS inline

    ap_uint<N> x0 = y;
    ap_uint<N> x1 = (x0 & 0x55555555) + ((x0 & 0xaaaaaaaa) >> 1);
    ap_uint<N> x2 = (x1 & 0x33333333) + ((x1 & 0xcccccccc) >> 2);
    ap_uint<N> x3 = (x2 & 0x0f0f0f0f) + ((x2 & 0xf0f0f0f0) >> 4);
    ap_uint<N> x4 = (x3 & 0x00ff00ff) + ((x3 & 0xff00ff00) >> 8);
    ap_uint<N> x5 = (x4 & 0x0000ffff) + ((x4 & 0xffff0000) >> 16);
    return x5;
}

int count_bits(long long x) {
    x = (x & 0x5555555555555555) + ((x & 0xaaaaaaaaaaaaaaaa) >> 1);
    x = (x & 0x3333333333333333) + ((x & 0xcccccccccccccccc) >> 2);
    x = (x & 0x0f0f0f0f0f0f0f0f) + ((x & 0xf0f0f0f0f0f0f0f0) >> 4);
    x = (x & 0x00ff00ff00ff00ff) + ((x & 0xff00ff00ff00ff00) >> 8);
    x = (x & 0x0000ffff0000ffff) + ((x & 0xffff0000ffff0000) >> 16);
    x = (x & 0x00000000ffffffff) + ((x & 0xffffffff00000000) >> 32);
    return x;
}

template <>
ap_uint<1> count_ones<1>(ap_uint<1> y) {
#pragma HLS inline

    return y & 0x1;
}
template <>
ap_uint<2> count_ones<2>(ap_uint<2> y) {
#pragma HLS inline

    ap_uint<2> x0 = y;
    ap_uint<2> x1 = (x0 & 0x1) + ((x0 & 0x2) >> 1);
    return x1;
}

template <>
ap_uint<4> count_ones<4>(ap_uint<4> y) {
#pragma HLS inline

    ap_uint<4> x0 = y;
    ap_uint<4> x1 = (x0 & 0x5) + ((x0 & 0xa) >> 1);
    ap_uint<4> x2 = (x1 & 0x3) + ((x1 & 0xc) >> 2);
    return x2;
}

template <>
ap_uint<8> count_ones<8>(ap_uint<8> y) {
#pragma HLS inline

    ap_uint<8> x0 = y;
    ap_uint<8> x1 = (x0 & 0x55) + ((x0 & 0xaa) >> 1);
    ap_uint<8> x2 = (x1 & 0x33) + ((x1 & 0xcc) >> 2);
    ap_uint<8> x3 = (x2 & 0x0f) + ((x2 & 0xf0) >> 4);
    return x3;
}

template <>
ap_uint<16> count_ones<>(ap_uint<16> y) {
#pragma HLS inline

    ap_uint<16> x0 = y;
    ap_uint<16> x1 = (x0 & 0x5555) + ((x0 & 0xaaaa) >> 1);
    ap_uint<16> x2 = (x1 & 0x3333) + ((x1 & 0xcccc) >> 2);
    ap_uint<16> x3 = (x2 & 0x0f0f) + ((x2 & 0xf0f0) >> 4);
    ap_uint<16> x4 = (x3 & 0x00ff) + ((x3 & 0xff00) >> 8);
    return x4;
}

template <>
ap_uint<32> count_ones<32>(ap_uint<32> y) {
#pragma HLS inline

    ap_uint<32> x0 = y;
    ap_uint<32> x1 = (x0 & 0x55555555) + ((x0 & 0xaaaaaaaa) >> 1);
    ap_uint<32> x2 = (x1 & 0x33333333) + ((x1 & 0xcccccccc) >> 2);
    ap_uint<32> x3 = (x2 & 0x0f0f0f0f) + ((x2 & 0xf0f0f0f0) >> 4);
    ap_uint<32> x4 = (x3 & 0x00ff00ff) + ((x3 & 0xff00ff00) >> 8);
    ap_uint<32> x5 = (x4 & 0x0000ffff) + ((x4 & 0xffff0000) >> 16);
    return x5;
}
template <>
ap_uint<128> count_ones<128>(ap_uint<128> y) {
#pragma HLS inline
    ap_uint<128> x0 = y;
    ap_uint<128> x1 = (x0 & 0x5555555555555555) + ((x0 & 0xaaaaaaaaaaaaaaaa) >> 1);
    ap_uint<128> x2 = (x1 & 0x3333333333333333) + ((x1 & 0xcccccccccccccccc) >> 2);
    ap_uint<128> x3 = (x2 & 0x0f0f0f0f0f0f0f0f) + ((x2 & 0xf0f0f0f0f0f0f0f0) >> 4);
    ap_uint<128> x4 = (x3 & 0x00ff00ff00ff00ff) + ((x3 & 0xff00ff00ff00ff00) >> 8);
    ap_uint<128> x5 = (x4 & 0x0000ffff0000ffff) + ((x4 & 0xffff0000ffff0000) >> 16);
    ap_uint<128> x6 = (x5 & 0x00000000ffffffff) + ((x5 & 0xffffffff00000000) >> 32);

    return x6;
}

} // utils_hw
} // common
} // xf

#ifndef __SYNTHESIS__
// for assert function.
#include <cassert>
#define XF_UTILS_HW_ASSERT(b) assert((b))
#else
#define XF_UTILS_HW_ASSERT(b) ((void)0)
#endif

#if __cplusplus >= 201103L
#define XF_UTILS_HW_STATIC_ASSERT(b, m) static_assert((b), m)
#else
#define XF_UTILS_HW_STATIC_ASSERT(b, m) XF_UTILS_HW_ASSERT((b) && (m))
#endif

#define XF_UTILS_HW_MACRO_QUOTE(s) #s
#define XF_UTILS_HW_MACRO_STR(s) XF_UTILS_HW_MACRO_QUOTE(s)

#endif // XF_UTILS_HW_COMMON_H
