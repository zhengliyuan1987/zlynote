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

.. _guide-stream_n_to_one_load_balance_type:

**************************************************************
Internals of stream_n_to_one_load_balance supporting any type
**************************************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_n_to_one distributing on load balance,
implemented as :ref:`stream_n_to_one <cid-xf::common::utils_hw::stream_n_to_one>` function.

.. image:: /images/stream_n_to_one/load_balance/stream_n_to_one_load_balance_type.png
   :alt: stream_n_to_one distribution on load balance Structure
   :width: 80%
   :align: center

The stream_n_to_one distributes the data from  n streams to one stream. Distribution on load balance means it is non-blocking. Compared to stream_n_to_one based on round robin, it skips to read the empty input streams. The data types of input and output are same.

