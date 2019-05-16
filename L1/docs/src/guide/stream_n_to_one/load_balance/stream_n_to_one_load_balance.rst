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

This document describes the structure and execution of stream_n_to_one distributing on load balance,
implemented as :ref:`stream_n_to_one <cid-xf::common::utils_hw::stream_n_to_one>` function.

.. image:: /images/stream_n_to_one/load_balance/stream_n_to_one_load_balance.png
   :alt: stream_n_to_one distribution on load balance Structure
   :width: 80%
   :align: center

The stream_n_to_one distributes the data from  n streams to one stream. Distribution on load balance means it is non-blocking. Compared to stream_n_to_one based on round robin, it skips to read the empty input streams. It also supports different input and output widths. It  gets all input streams' states ahead, then reads  data from not empty streams to a buffer. If the total available bits in buffer is enough, then output a data. 
For example, n = 4, input width(written as win) =16, output width(written as wout)=128.  
At c0 cycle,  1# stream is empty,  then read 48bits data from 0#,2# and 3# stream,  no output at this cycle;  at c1 cycle,  2# is empty, read 48bits from 0#, 1# and 3#stream; at c3 cycle, 1# and 2# stream are empty, read 32 bits from 0# and 3# stream, then output them as a data by the output stream because of total read 48+48+32=128 bits data from input streams. In the process, put the data from 0# at c0 on the lowest 16bit in the output data, and 3# at c3 on the highest 16 bits. 

There are Applicable conditions:

1. The leat common multiple(lcm) of input width and n * output width is no more than 4096.

   For better performance, AP_INT_MAX_W is defined to 4096 in type.h in default, which will be changed manually if lcm is more than 4096. However, it must be less than 32768.

2. n is more than 1. It doesn't work when one stream is distributed to another one on load balance.

The design of the primitive includes 3 modules:

1. read: Read data from the n input streams then output data by one stream whose width is (n * win) bits. 

2. collect: The least common multiple of  n * win and wout is the inner buffer size in order to solve the different input width and output width.
        
                buf_size = lcm ( n * win, wout)

        Collect buf_size/(n*win)  input data (each has n*win bits) to a buffer , then output them at once to a stream.

3. distribute:  Read a data with buf_size bits from input stream, then output  buf_size/wout  times to ostrm.



.. image:: /images/stream_n_to_one/load_balance/stream_n_to_one_load_balance_detail.png
   :alt:  design details of n streams to one distribution on load balance
   :width: 100%
   :align: center



.. CAUTION::
   These Applicable conditions.


