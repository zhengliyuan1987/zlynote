#ifndef XF_UTIL_ENUMS_H
#define XF_UTIL_ENUMS_H

namespace xf {
namespace common {
namespace utils_hw {

// Use argument type to select function,
// to avoid trouble with template inference.

// distribute and collect
struct tag_select_t {};
struct load_balance_t {};
struct round_robin_t {};

// combine
struct combine_left_t {};
struct combine_right_t {};

// shuffle
struct shuffle_left_t {};
struct shuffle_right_t {};

} // utils_hw
} // common
} // xf

#endif // XF_UTIL_ENUMS_H
