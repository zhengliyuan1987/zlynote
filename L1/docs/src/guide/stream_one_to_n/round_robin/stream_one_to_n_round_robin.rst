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

.. _guide-stream_one_to_n:

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

For example, n = 4, input width(written as win) =64, output width(written as wout)=16,  the lowest 16 bits in the input data is write to 0#stream, and the highest 16 bits to 3#stream. If win=128, output the lowest 16 bits to 0#stream at c0 cycle, and the highest 16bit to 3# stream at c1 cycle. 
When  win > n*wout, output times is more than input ones, so read should wait a few cycles sometimes in order to output all buffered data.  
When  win < n*wout, output times is less than input ones, so write should wait a few cycles sometimes in order to read enough data from input streams.  

There are Applicable conditions:

   The leat common multiple(lcm) of input width and n * output width is no more than 4096.

   For better performance, AP_INT_MAX_W is defined to 4096 in type.h in default, which will be changed manually if lcm is more than 4096. However, it must be less than 32768.

The design of the primitive includes 3 modules:

1. read: Read data from the input stream then output data by one stream whose width is lcm( win, n * wout) bits. Here, the least common multiple of win and n*wout is the inner buffer size in order to solve the different input width and output width. 

                buf_size = lcm (win, n * wout)

2. reduce: split the large width to a few of n*wout bits in order to distribute.
        
        Get a data whose has buf_size bits and output buf_size/(n*wout) times data which has n*wout bits.

3. distribute:  Read a data with n*wout bits from input stream, then slit to n data which are distribution on round robin.


.. image:: /images/stream_one_to_n/round_robin/stream_one_to_n_round_robin_detail.png
   :alt:  design details of n streams to one distribution on round robin
   :width: 100%
   :align: center
 

.. CAUTION::
   These Applicable conditions.

