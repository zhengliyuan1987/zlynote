#ifndef XF_UTIL_ENUMS_H
#define XF_UTIL_ENUMS_H

namespace xf {
namespace util {

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

} // util
} // xf

static const xf::util::tag_select_t tag_select {};
static const xf::util::load_balance_t load_balance {};
static const xf::util::round_robin_t round_robin {};

static const xf::util::combine_left_t combine_left {};
static const xf::util::combine_right_t combine_right {};

static const xf::util::shuffle_left_t shuffle_left {};
static const xf::util::shuffle_right_t shuffle_right {};

#endif // XF_UTIL_ENUMS_H
