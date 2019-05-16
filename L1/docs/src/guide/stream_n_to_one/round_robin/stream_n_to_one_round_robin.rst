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

.. _guide-stream_n_to_one:

*****************************************
Internals of stream_n_to_one_round_robin
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_n_to_one distributing on round robin,
implemented as :ref:`stream_n_to_one <cid-xf::common::utils_hw::stream_n_to_one>` function.

.. image:: /images/stream_n_to_one/round_robin/stream_n_to_one_round_robin.png
   :alt: n streams to one based on round robin Structure
   :width: 80%
   :align: center

The stream_n_to_one distributes the data from  n streams to one stream.  This primitive supports different input and output width.  In the process, read the input data to a buffer follows  the order: 0#stream, 1#stream, 2#stream, ... , (n-1)#stream, 0#stream, 1#stream, etc while output new data when the total available bits are enough data (up to output width bits).  

For example, n = 4, input width(written as win) =16, output width(written as wout)=64,  the lowest 16 bits in  output data is the data from 0#stream, and the highest 16 bits from 3#stream. 
When n* win > wout, output times is more than input ones, so read should wait a few cycles sometimes in order to output all buffered data.  
When n* win < wout, output times is less than input ones, so output should wait a few cycles sometimes in order to read enough data from input streams.  

There are Applicable conditions:

   The leat common multiple(lcm) of input width and n * output width is no more than 4096.

   For better performance, AP_INT_MAX_W is defined to 4096 in type.h in default, which will be changed manually if lcm is more than 4096. However, it must be less than 32768.

The design of the primitive includes 3 modules:

1. read: Read data from the n input streams then output data by one stream whose width is (n * win) bits. 

2. collect: The least common multiple of  n * win and wout is the inner buffer size in order to solve the different input width and output width.
        
                buf_size = lcm ( n * win, wout)

        Collect buf_size/(n*win)  input data (each has n*win bits) to a buffer , then output them at once to a stream.

3. distribute:  Read a data with buf_size bits from input stream, then output  buf_size/wout  times to ostrm.

.. image:: /images/stream_n_to_one/round_robin/stream_n_to_one_round_robin_detail.png
   :alt:  design details of n streams to one based on round robin
   :width: 100%
   :align: center
 

.. CAUTION::
   These Applicable conditions.

