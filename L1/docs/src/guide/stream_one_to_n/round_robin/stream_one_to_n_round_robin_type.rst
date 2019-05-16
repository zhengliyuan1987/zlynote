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

*************************************************
Internals of stream_one_to_n supporting any type
*************************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_one_to_n supporting any type which distributes on round robin,
implemented as :ref:`stream_one_to_n <cid-xf::common::utils_hw::stream_one_to_n>` function.

.. image:: /images/stream_one_to_n/round_robin/stream_one_to_n_round_robin_type.png
   :alt: two types of axi_to_stream Structure
   :width: 80%
   :align: center

This stream_one_to_n supports any type and distributes data in the order of round robin from input streams. The data types of input stream and output streams are same.  


.. CAUTION::
    No conditions.


