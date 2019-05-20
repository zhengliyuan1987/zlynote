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

.. _guide-stream_combine_lsb:

*****************************************
Internals of stream_combine_lsb
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_combine from LSB,
implemented as :ref:`stream_combine <cid-xf::common::utils_hw::stream_combine>` function.

.. image:: /images/stream_combine/stream_combine_lsb.png
   :alt: combination n streams to one from LSB Structure
   :width: 80%
   :align: center

The stream_combine combines n streams into one stream. Combination from LSB means filling output data from the lowest bit. 

For example, n = 4, input width(written as win) =16, output width(written as wout)=64, read data from input streams then the data from 0#stream fill the lowest 16bits and the data from 3# fill the highest 16bits. If wout=72, then the lowest 64bits(4*16bits) are filled by input streams  and the highest 8bits are 0.

There is an  Applicable condition:

   n*win <= wout


.. CAUTION::
   The Applicable condition.

