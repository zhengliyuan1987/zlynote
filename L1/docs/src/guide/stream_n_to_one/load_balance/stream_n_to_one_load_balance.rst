.. 
   Copyright 2019 Xilinx, Inc.
  
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
  
       http://www.apache.org/licenses/LICENSE-2.0
  
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

.. _guide-stream_n_to_one_load_balance:

*******************************************
Internals of stream_n_to_one_load_balance
*******************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream n to one,
implemented as :ref:`stream_n_to_one <cid-xf::common::utils_hw::stream_n_to_one>` function.

.. image:: /images/stream_n_to_one/load_balance/stream_n_to_one_load_balance.png
   :alt: stream_n_to_one based on load balance Structure
   :width: 80%
   :align: center

The stream_n_to_one for aligned data implement is a lightweight primitive for aligned data, the width of AXI port
is positive integer multiple of alignment width and the stream's width just equals the aligned width. Both AXI port 
and alignment width are assumed to be multiple of 8-bit char.
The axi_to_stream for general data is relatively universal compared with the axi_to_stream for aligned data,
so it causes more resource. The data length should be in number of char.the data width cloud be unaligned or aligned,
e.g. compressed binary files. AXI port is assumed to have width as multiple of 8-bit char.

There are Applicable conditions:

1. When input data ptr width is less than AXI port width, the AXI port bandwidth
   will not be fully used. So, AXI port width should be minimized while meeting
   performance requirements of application.

This primitive performs axi_to_stream in two modules working simultaneously. For example, in a <_WAxi, _BstLen, _TStrm>=<512,32,ap_uint<64> >
for binary files design:

1. read_to_vec: read axi ptr to a _WAxi width stream

2. split_vec_to_aligned: It takes the _WAxi width stream, aligned to the stream width, then split the _WAxi width data to 
   stream width and output

.. CAUTION::
   These Applicable conditions.

This ``stream_n_to_one`` primitve has only one port for axi ptr and one port for stream output.

