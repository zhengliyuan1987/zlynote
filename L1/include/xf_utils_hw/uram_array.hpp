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

/**
 * @file uram_array.hpp
 * @brief URAM_ARRAY template function implementation.
 *
 * This file is part of XF Common Utils Library.
 */

#ifndef XF_UTILS_HW_URAM_ARRAY_H
#define XF_UTILS_HW_URAM_ARRAY_H

#include <ap_int.h>

namespace xf {
namespace common {
namespace utils_hw {

namespace details {
/// @brief the structure of compute available uram block numbers.
///
/// @tparam width width of one data.
/// @tparam depth size of one array.
/// @tparam B     condition of W <= 72.
template <int total, int one, bool B = (total <= 72)>
struct need_num {
 private:
  static const int elem_per_line = 72 / total;

 public:
  static const int value =
      ((total + 71) / 72) *
      ((one + (elem_per_line * 4096) - 1) / (elem_per_line * 4096));
};

template <int total, int one>
struct need_num<total, one, false> {
 public:
  static const int value = ((total + 71) / 72) * ((one + 4095) / 4096);
};
} // details

/// @class uram_array uram_array.h "xf_utils_hw/uram_array.h"
///
/// @brief Helper class to create URAM array that can be updated every cycle
/// with forwarding regs.
///
/// @tparam _WData  the width of every element.
/// @tparam _NData  the number of elements in the array.
/// @tparam _NCache the number of cache.
template <int _WData, int _NData, int _NCache>
class uram_array {
 public:
  uram_array() {
#pragma HLS RESOURCE variable = blocks core = XPM_MEMORY uram
#pragma HLS ARRAY_PARTITION variable = blocks complete dim = 1
#pragma HLS ARRAY_PARTITION variable = _index complete dim = 1
#pragma HLS ARRAY_PARTITION variable = _state complete dim = 1

#ifndef __SYNTHESIS__
    for (int i = 0; i < _num_uram_block; i++) {
      blocks[i] = (ap_uint<72>*)malloc(sizeof(ap_uint<72>) * 4096);
    }
#endif
    for (int i = 0; i < _NCache; i++) {
#pragma HLS unroll
      _index[i] = -1;
      _state[i] = 0;
    }
  }

  ~uram_array() {
#ifndef __SYNTHESIS__
    for (int i = 0; i < _num_uram_block; i++) {
      free(blocks[i]);
    }
#endif
  }

  /// @brief  initialization for uram.
  /// @param  d value for initialization.
  /// @return number of block which had been initialize.
  int memset(const ap_uint<_WData>& d);

  /// @brief write to uram.
  /// @param index the index which you want to write.
  /// @param d     the value what you want to write.
  void write(int index, const ap_uint<_WData>& d);

  /// @brief  read from uram.
  /// @param  index the index which you want to read.
  /// @return value you had read.
  ap_uint<_WData> read(int index);

 private:
  /// number elements per line, used with _WData<=72. For example, when _WData =
  /// 16, _elem_per_line is 4.
  static const int _elem_per_line;

  /// piece of block using one time.
  static const int _num_parallel_block;

  /// index numbers of one block.
  static const int _elem_per_block;

  /// the numbers of need to access.
  static const int _num_uram_block;

  /// the cache for saving latest index.
  int _index[_NCache];

  /// the cache for saving latest value.
  ap_uint<_WData> _state[_NCache];

 public:
// XXX this has to be public, otherwise `depencency false` cannot be
// specified.
#ifndef __SYNTHESIS__
  ap_uint<72>* blocks[details::need_num<_WData, _NData>::value];
#else
  ap_uint<72> blocks[details::need_num<_WData, _NData>::value][4096];
#endif
};

// Const member variables

template <int _WData, int _NData, int _NCache>
const int uram_array<_WData, _NData, _NCache>::_elem_per_line = (72 / _WData);

template <int _WData, int _NData, int _NCache>
const int uram_array<_WData, _NData, _NCache>::_elem_per_block = (_elem_per_line *
                                                               4096);

template <int _WData, int _NData, int _NCache>
const int uram_array<_WData, _NData, _NCache>::_num_parallel_block =
    ((_WData + 71) / 72);

template <int _WData, int _NData, int _NCache>
const int uram_array<_WData, _NData, _NCache>::_num_uram_block =
    (details::need_num<_WData, _NData>::value);

// Methods

template <int _WData, int _NData, int _NCache>
int uram_array<_WData, _NData, _NCache>::memset(const ap_uint<_WData>& d) {
  if (_num_uram_block == 0) return 0;

  ap_uint<72> t;
l_init_value:
  for (int i = 0; i < 4096; i++) {
#pragma HLS PIPELINE II = 1
    for (int nb = 0; nb < _num_uram_block; nb++) {
#pragma HLS unroll
      if (_WData <= 72) {
        for (int j = 0; j < _elem_per_line; j++) {
#pragma HLS unroll
          t(j * _WData + _WData - 1, j * _WData) = d(_WData - 1, 0);
        }
        blocks[nb][i] = t;
      } else {
        int offset = nb % _num_parallel_block;
        int begin = offset * 72;
        if (offset == (_num_parallel_block - 1))
          blocks[nb][i] = d(_WData - 1, begin);
        else
          blocks[nb][i] = d(begin + 71, begin);
      }
    }
  }

// update d for cache
init_cache:
  for (int i = 0; i < _NCache; i++) {
#pragma HLS unroll
    _state[i] = d;
    _index[i] = 0;
  }

  return _num_uram_block;
}

template <int _WData, int _NData, int _NCache>
void uram_array<_WData, _NData, _NCache>::write(int index,
                                             const ap_uint<_WData>& d) {
#pragma HLS inline
  int div_block = 0, div_index = 0;
  int dec_block = 0, dec, begin;

Write_Inner:
  for (int i = 0; i < _num_parallel_block; i++) {
    if (_WData <= 72) {
      div_block = index / _elem_per_block;
      dec_block = index % _elem_per_block;
      div_index = dec_block / _elem_per_line;
      dec = dec_block % _elem_per_line;
      begin = dec * _WData;
      ap_uint<72> tmp = blocks[div_block][div_index];
      tmp.range(begin + _WData - 1, begin) = d;
      blocks[div_block][div_index] = tmp;
    } else {
      div_block = index / 4096;
      dec_block = index % 4096;
      int ii = i + div_block * _num_parallel_block;
      begin = i * 72;
      if (i == (_num_parallel_block - 1))
        blocks[ii][dec_block] = d.range(_WData - 1, begin);
      else
        blocks[ii][dec_block] = d.range(begin + 71, begin);
    }
  }

Write_Cache:
  for (int i = _NCache - 1; i >= 1; i--) {
    _state[i] = _state[i - 1];
    _index[i] = _index[i - 1];
  }
  _state[0] = d;
  _index[0] = index;
}

template <int _WData, int _NData, int _NCache>
ap_uint<_WData> uram_array<_WData, _NData, _NCache>::read(int index) {
#pragma HLS inline
  ap_uint<_WData> value;
  int div_block = 0, div_index = 0;
  int dec_block = 0, dec, begin;

Read_Cache:
  for (int i = 0; i < _NCache; i++) {
    if (index == _index[i]) {
      value = _state[i];
      return value;
    }
  }

Read_Inner:
  for (int i = 0; i < _num_parallel_block; i++) {
    if (_WData <= 72) {
      div_block = index / _elem_per_block;
      dec_block = index % _elem_per_block;
      div_index = dec_block / _elem_per_line;
      dec = dec_block % _elem_per_line;
      begin = dec * _WData;
      ap_uint<72> tmp = blocks[div_block][div_index];
      value = tmp.range(begin + _WData - 1, begin);
    } else {
      div_block = index / 4096;
      dec_block = index % 4096;
      int ii = i + div_block * _num_parallel_block;
      begin = i * 72;

      if (i == (_num_parallel_block - 1))
        value.range(_WData - 1, begin) = blocks[ii][dec_block];
      else {
        value.range(begin + 71, begin) = blocks[ii][dec_block];
      }
    }
  }
  return value;
}

} // utils_hw
} // common
} // xf

#endif
