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

.. _guide-stream_discard_single:

*****************************************
Internals of stream_discard_single
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_discard for single input stream,
implemented as :ref:`stream_discard <cid-xf::common::utils_hw::stream_discard>` function.

.. image:: /images/stream_discard/stream_discard_single.png
   :alt: discarding a singe stream structure
   :width: 80%
   :align: center

The stream_discard reads the input stream but no output.That is to say, it discards all input elements. 
This primitive include a input stream and its end-flag stream.

.. CAUTION::
   Applicable condition:
   The length of end-flag stream is 1 more than the one of input data stream.

