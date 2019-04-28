#ifndef XF_UTIL_UTILS_H
#define XF_UTIL_UTILS_H

template <int _N>
struct PowerOf2 {
  static const unsigned value = (1u << _N);
};

#endif // XF_UTIL_UTILS_H
