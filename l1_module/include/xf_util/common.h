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

#ifndef __SYNTHESIS__
// for assert function.
#include <cassert>
#define XF_UTIL_ASSERT(b) assert((b))
#else
#define XF_UTIL_ASSERT(b) ((void)0)
#endif

#if __cplusplus >= 201103L
#define XF_UTIL_STATIC_ASSERT(b, m) static_assert((b), m)
#else
#define XF_UTIL_STATIC_ASSERT(b, m) XF_UTIL_ASSERT((b) && (m))
#endif

#define XF_UTIL_MACRO_QUOTE(s) #s
#define XF_UTIL_MACRO_STR(s) XF_UTIL_MACRO_QUOTE(s)


#endif // XF_UTIL_COMMON_H
