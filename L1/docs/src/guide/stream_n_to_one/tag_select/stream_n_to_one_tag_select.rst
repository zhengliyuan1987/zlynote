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

.. _guide-stream_n_to_one_tag_select:

*****************************************
Internals of stream_n_to_one_tag_select
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_n_to_one distribution on tag,
implemented as :ref:`stream_n_to_one <cid-xf::common::utils_hw::stream_n_to_one>` function.

.. image:: /images/stream_n_to_one/tag_select/stream_n_to_one_tag_select.png
   :alt: n streams to one distribution on tag Structure
   :width: 80%
   :align: center

The stream_n_to_one distributes the data from n streams to one stream.  Distribution on tag means output data depend on tag, which is the index of input streams.

For example, there are 4 input streams and tag is 2, then read the third input stream and output it. 

There is an Applicable condition:

   This primitive  only supports the data type of ap_uint<>.

.. CAUTION::
   The Applicable condition.

