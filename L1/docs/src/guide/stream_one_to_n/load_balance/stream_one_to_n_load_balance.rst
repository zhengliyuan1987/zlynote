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

.. _guide-stream_one_to_n_load_balance:

*******************************************
Internals of stream_one_to_n_load_balance
*******************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_one_to_n distribution on load balance,
implemented as :ref:`stream_one_to_n <cid-xf::common::utils_hw::stream_one_to_n>` function.

.. image:: /images/stream_one_to_n/load_balance/stream_one_to_n_load_balance.png
   :alt: stream_one_to_n distribution on load balance Structure
   :width: 80%
   :align: center

The stream_one_to_n distributes the data from  one stream to n streams. Distribution on load balance means it is non-blocking. Compared to stream_one_to_n based on round robin, it skips to write the full input streams. It also supports different input and output widths.
For example, n = 4, input width(written as win) =128, output width(written as wout)=16.  
At c0 cycle,read a data from input stream, asssuming  1# stream is full,  then write 0#,2# and 3# stream; at c1 cycle,  2# is full, write 0#, 1# and 3#stream; at c3 cycle, 1# and 2# stream are full, write 0# and 3# stream. In the process, output  the lowest 16bit in the input data to 0# at c0,  and the highest 16 bits to 3# at c3. 

There are Applicable conditions:

1. The leat common multiple(lcm) of input width and n * output width is no more than 4096.

   For better performance, AP_INT_MAX_W is defined to 4096 in type.h in default, which will be changed manually if lcm is more than 4096. However, it must be less than 32768.

2. n is more than 1. It doesn't work when one stream is distributed to another one on load balance.

The design of the primitive includes 3 modules:

1. read: Read data from the input stream then output data by one stream whose width is lcm( win, n * wout) bits. Here, the least common multiple of win and n*wout is the inner buffer size in order to solve the different input width and output width. 

                buf_size = lcm ( win, n * wout)

2. reduce: split the large width to a few of n*wout bits in order to distribute.
        
        Get a data whose has buf_size bits and output buf_size/(n*wout) times data which has n*wout bits.

3. distribute:  Read a data with n*wout bits from input stream, then slit to n data which are distribution on load balance.



.. image:: /images/stream_one_to_n/load_balance/stream_one_to_n_load_balance_detail.png
   :alt:  design details of n streams to one distribution on load balance
   :width: 100%
   :align: center



.. CAUTION::
   These Applicable conditions.


