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

.. _stream_based:

***************************************************
Stream-Based API Design
***************************************************

.. toctree::
   :maxdepth: 1

Stream-based Interface
----------------------

The interface of primitives in this library are mostly HLS streams, with a single bit stream
along with the main data stream throughout the dataflow.

.. code:: cpp

        hls::stream<ap_uint<W> >& data_strm,
        hls::stream<bool>&        e_data_strm,

The benefits of this interface are

* Within a HLS dataflow region, all primitives connected via HLS streams can work in
  parallel, and this is the key to FPGA acceleration.

* Using the single bit stream to mark *end of operation* can trigger stream consumer
  as soon as the first row data becomes available, without known how many rows will be
  generated later. Moreover, it can represent empty table.


.. ERROR:: TODO More details needed here.
