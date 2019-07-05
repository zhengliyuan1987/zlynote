#ifndef XF_UTILS_HW_ENUMS_H
#define XF_UTILS_HW_ENUMS_H

/**
 * @file enums.hpp
 * @brief Enum and tag-class definition.
 *
 * This file is part of XF Hardware Utilities Library.
 */

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
struct lsb_side_t {};
struct msb_side_t {};

//shuffle
struct shuffle_left_t {};
struct shuffle_right_t {};

} // utils_hw
} // common
} // xf

#endif // XF_UTILS_HW_ENUMS_H
