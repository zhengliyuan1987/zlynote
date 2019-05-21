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

.. _guide-stream_discard_streams:

*****************************************
Internals of stream_discard_streams
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_discard for streams,
implemented as :ref:`stream_discard <cid-xf::common::utils_hw::stream_discard>` function.

.. image:: /images/stream_discard/stream_discard_array.png
   :alt: discarding streams structure
   :width: 80%
   :align: center

The stream_discard reads all input streams but no output.That is to say, it discards all input elements. 
The primitive includes multiple input streams and end-flag streams. Each input stream and its end-flag have no dependency on others.

.. CAUTION::
   Applicable condition:
   Each input data stream has an end-flag stream, and the length of the former are 1 more than the one of the latter.

