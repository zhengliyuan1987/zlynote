#ifndef XF_UTIL_COMMON_H
#define XF_UTIL_COMMON_H

namespace xf {
namespace util {

// Greatest Common Divisor
template<int a,int b> struct gcd{
     static const int value= gcd<b, a % b>::value;
};
template<int a> struct gcd<a,0> {
     static const int value=a;
};
// Least Common Multiple
template<int a,int b> struct lcm{
     static const int value= a * b / gcd<b, a % b>::value;
};
} //util
} //xf
#endif // XF_UTIL_COMMON_H
