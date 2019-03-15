#ifndef XF_DATA_UTILS_UTILS_H
#define XF_DATA_UTILS_UTILS_H

template <int n>
struct power_of_2 {
  static const unsigned value = (1u << n);
}

#endif // XF_DATA_UTILS_UTILS_H
