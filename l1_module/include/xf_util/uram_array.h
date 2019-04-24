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
 * @file uram_array.h
 * @brief URAM_ARRAY template function implementation.
 *
 * This file is part of XF Common Utils Library.
 */

#ifndef XF_DATABASE_URAM_ARRAY_H
#define XF_DATABASE_URAM_ARRAY_H

#ifndef __cplusplus
#error "xf_util_level1 uram_array interface, and thus requires C++"
#endif

#include <ap_int.h>
#include <iostream>
#include <math.h>

namespace xf {
namespace util {
namespace level1 {

/// @brief the structure of compute available uram block numbers.

/// @tparam width width of one data.
/// @tparam depth size of one array.
/// @tparam B     condition of W <= 72.
template <int W, int N, bool B = (W <= 72)> struct need_num {
private:
  static const int elem_per_line = 72 / W;

public:
  static const int value = ((W + 71) / 72) * ((N + (elem_per_line * 4096) - 1) /
                                              (elem_per_line * 4096));
};

template <int W, int N> struct need_num<W, N, false> {
public:
  static const int value = ((W + 71) / 72) * ((N + 4095) / 4096);
};

/// @brief URAMs are 72 bit fixed, and currently very large buffers,
/// needs logic help to be read/write every cycle.

/// @tparam WData  the width of every element.
/// @tparam NData  the depth of array.
/// @tparam NCache the number of cache.
template <int WData, int NData, int NCache> class uram_array {
public:
  uram_array()
      : _elem_per_line(72 / WData), _num_one_process((WData + 71) / 72),
        _index_one_block(_elem_per_line * 4096) {
#pragma HLS RESOURCE variable = _blocks core = XPM_MEMORY uram
#pragma HLS ARRAY_PARTITION variable = _blocks complete dim = 1
#pragma HLS ARRAY_PARTITION variable = _index complete dim = 1
#pragma HLS ARRAY_PARTITION variable = _state complete dim = 1
#ifndef __SYNTHESIS__
    // TODO:malloc the array
    for (int i = 0; i < _num_uram_block; i++) {
      _blocks[i] = (ap_uint<72> *)malloc(sizeof(ap_uint<72>) * 4096);
    }
#endif
    for (int i = 0; i < NCache; i++) {
#pragma HLS unroll
      _index[i] = -1;
      _state[i] = 0;
    }
  }
  ~uram_array() {
#ifndef __SYNTHESIS__
  Delete_Loop:
    for (int i = 0; i < _num_uram_block; i++) {
      free(_blocks[i]);
    }
#endif
  }

  /// @brief  initialization for uram.
  /// @param  d value for initialization.
  /// @return number of block which had been initialize.
  int memset_uram(const ap_uint<WData> &d);

  /// @brief write to uram.
  /// @param index the index which you want to write.
  /// @param d     the value what you want to write.
  void write(int index, const ap_uint<WData> &d);

  /// @brief  read from uram.
  /// @param  index the index which you want to read.
  /// @return value you had read.
  ap_uint<WData> read(int index);

private:
  /// number elements per line, used with WData<=72. For example, when WData =
  /// 16, _elem_per_line is 4.
  const int _elem_per_line;

  /// piece of block using one time.
  const int _num_one_process;

  /// index numbers of one block.
  const int _index_one_block;

  /// the numbers of need to access.
  static const int _num_uram_block;

  /// the cache for saving latest index.
  int _index[NCache];

  /// the cache for saving latest value.
  ap_uint<WData> _state[NCache];

public:
/// the memory of accessing,one block's fixed width is 72 and the depth is 4K.
#ifndef __SYNTHESIS__
  ap_uint<72> *_blocks[need_num<WData, NData>::value];
#else
  ap_uint<72> _blocks[need_num<WData, NData>::value][4096];
#endif
};

template <int WData, int NData, int NCache>
const int uram_array<WData, NData, NCache>::_num_uram_block =
    need_num<WData, NData>::value;

template <int WData, int NData, int NCache>
int uram_array<WData, NData, NCache>::memset_uram(const ap_uint<WData> &d) {
  if (_num_uram_block == 0)
    return 0;

  ap_uint<72> t;
l_init_value:
  for (int i = 0; i < 4096; i++) {
#pragma HLS PIPELINE II = 1
    for (int nb = 0; nb < _num_uram_block; nb++) {
#pragma HLS unroll
      if (WData <= 72) {
        for (int j = 0; j < _elem_per_line; j++) {
#pragma HLS unroll
          t(j * WData + WData - 1, j * WData) = d(WData - 1, 0);
        }
        _blocks[nb][i] = t;
      } else {
        int offset = nb % _num_one_process;
        int begin = offset * 72;
        if (offset == (_num_one_process - 1))
          _blocks[nb][i] = d(WData - 1, begin);
        else
          _blocks[nb][i] = d(begin + 71, begin);
      }
    }
  }

  // update d for cache
  if (WData <= 72)
    _index[0] = _elem_per_line * _num_uram_block * 4096 - 1;
  else
    _index[0] = _num_uram_block / _num_one_process * 4096 - 1;
  _state[0] = d;
init_cache:
  for (int i = 1; i < NCache; i++) {
#pragma HLS unroll
    _state[i] = d;
    _index[i] = _index[i - 1] - 1;
  }

  return _num_uram_block;
}

template <int WData, int NData, int NCache>
void uram_array<WData, NData, NCache>::write(int index,
                                             const ap_uint<WData> &d) {
#pragma HLS inline
  int div_block = 0, div_index = 0;
  int dec_block = 0, dec, begin;

Write_Inner:
  for (int i = 0; i < _num_one_process; i++) {
    if (WData <= 72) {
      div_block = index / _index_one_block;
      dec_block = index % _index_one_block;
      div_index = dec_block / _elem_per_line;
      dec = dec_block % _elem_per_line;
      begin = dec * WData;
      ap_uint<72> tmp = _blocks[div_block][div_index];
      tmp.range(begin + WData - 1, begin) = d;
      _blocks[div_block][div_index] = tmp;
    } else {
      div_block = index / 4096;
      dec_block = index % 4096;
      int ii = i + div_block * _num_one_process;
      begin = i * 72;
      if (i == (_num_one_process - 1))
        _blocks[ii][dec_block] = d.range(WData - 1, begin);
      else
        _blocks[ii][dec_block] = d.range(begin + 71, begin);
    }
  }

Write_Cache:
  for (int i = NCache - 1; i >= 1; i--) {
    _state[i] = _state[i - 1];
    _index[i] = _index[i - 1];
  }
  _state[0] = d;
  _index[0] = index;
}

template <int WData, int NData, int NCache>
ap_uint<WData> uram_array<WData, NData, NCache>::read(int index) {
#pragma HLS inline
  ap_uint<WData> value;
  int div_block = 0, div_index = 0;
  int dec_block = 0, dec, begin;

Read_Cache:
  for (int i = 0; i < NCache; i++) {
    if (index == _index[i]) {
      value = _state[i];
      return value;
    }
  }

Read_Inner:
  for (int i = 0; i < _num_one_process; i++) {
    if (WData <= 72) {
      div_block = index / _index_one_block;
      dec_block = index % _index_one_block;
      div_index = dec_block / _elem_per_line;
      dec = dec_block % _elem_per_line;
      begin = dec * WData;
      ap_uint<72> tmp = _blocks[div_block][div_index];
      value = tmp.range(begin + WData - 1, begin);
    } else {
      div_block = index / 4096;
      dec_block = index % 4096;
      int ii = i + div_block * _num_one_process;
      begin = i * 72;

      if (i == (_num_one_process - 1))
        value.range(WData - 1, begin) = _blocks[ii][dec_block];
      else {
        value.range(begin + 71, begin) = _blocks[ii][dec_block];
      }
    }
  }

  return value;
}

} // level1
} // dal
} // xf

#endif
