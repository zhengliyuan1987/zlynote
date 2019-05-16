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

.. _guide-stream_one_to_n_tag_select:

*****************************************
Internals of stream_one_to_n_tag_select
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_one_to_n distribution on tag,
implemented as :ref:`stream_one_to_n <cid-xf::common::utils_hw::stream_one_to_n>` function.

.. image:: /images/stream_one_to_n/tag_select/stream_one_to_n_tag_select.png
   :alt: one stream to n distribution on tag Structure
   :width: 80%
   :align: center

The stream_one_to_n distributes the data from one stream to one streams.  Distribution on tag means output data depend on tag, which is the index of output streams.

For example, there are 4 output streams and tag is 2, then write the third stream. 

There is an Applicable condition:

   This primitive  only supports the data type of ap_uint<>.

.. CAUTION::
   The Applicable condition.

