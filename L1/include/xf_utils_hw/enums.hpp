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
struct TagSelectT {};
struct LoadBalanceT {};
struct RoundRobinT {};

// combine
struct LSBSideT {};
struct MSBSideT {};

// shuffle
struct ShuffleLeftT {};
struct ShuffleRightT {};

} // utils_hw
} // common
} // xf

#endif // XF_UTILS_HW_ENUMS_H
