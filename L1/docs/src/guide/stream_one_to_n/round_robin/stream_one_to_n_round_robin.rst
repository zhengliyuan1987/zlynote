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

.. _guide-stream_one_to_n_round_robin:

*****************************************
Internals of stream_one_to_n_round_robin
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_one_to_n distribution on round robin,
implemented as :ref:`stream_one_to_n <cid-xf::common::utils_hw::stream_one_to_n>` function.

.. image:: /images/stream_one_to_n/round_robin/stream_one_to_n_round_robin.png
   :alt: one stream to n distribution on round robin Structure
   :width: 80%
   :align: center

The stream_one_to_n distributes the data from  one stream to n streams.  This primitive supports different input and output width.  In the process, write the output streams follows  the order: 0#stream, 1#stream, 2#stream, ... , (n-1)#stream, 0#stream, 1#stream, etc.  

For example, n = 8, input width(written as win) =64, output width(written as wout)=16,  read a data from input stream then its lowest 16 bits is write to 0#stream, and its highest 16 bits to 3#stream; read a new data, the lowest 16 bits to 4# and the highest ones to 7#.

When  win > n*wout, output times is more than input ones, so read should wait a few cycles sometimes in order to output all buffered data.  
When  win < n*wout, output times is less than input ones, so write should wait a few cycles sometimes in order to read enough data from input streams.  

There are Applicable conditions:

   The leat common multiple(lcm) of input width and n * output width is no more than 4096.

   For better performance, AP_INT_MAX_W is defined to 4096 in type.h in default, which will be changed manually if lcm is more than 4096. However, it must be less than 32768.

The design of the primitive applies ping-pong buffers to obtain high throughput and the least common multiple of win and n*wout is the inner buffer size in order to solve the different input width and output width. 

                buf_size = lcm (win, n * wout)
At each time, one buffer is storing win bits data from input stream while n*wout bits from another one are output to n streams. 

.. image:: /images/stream_one_to_n/round_robin/stream_one_to_n_round_robin_detail.png
   :alt:  design details of n streams to one distribution on round robin
   :width: 100%
   :align: center
 

.. CAUTION::
   These Applicable conditions.

