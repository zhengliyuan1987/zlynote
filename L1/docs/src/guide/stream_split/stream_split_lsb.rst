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

.. _guide-stream_split_lsb:

*****************************************
Internals of stream_split_lsb
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_split from LSB,
implemented as :ref:`stream_split <cid-xf::common::utils_hw::stream_split>` function.

.. image:: /images/stream_split/stream_split_lsb.png
   :alt: one stream to n distribution on MSB Structure
   :width: 80%
   :align: center

The stream_split splits one wide stream into multiple streams. Split from LSB means output from the lowest bit. 

For example, n = 4, input width(written as win) =64, output width(written as wout)=16, read a data from input stream then its lowest 16bits are written to 0#stream, and its highest 16 bits to 3#stream. If win=72, then the lowest 64bits(4*16bits) are output and the highst 8bits are discarded.

There is an  Applicable condition:

   win >= n*wout


.. CAUTION::
   The Applicable condition.

