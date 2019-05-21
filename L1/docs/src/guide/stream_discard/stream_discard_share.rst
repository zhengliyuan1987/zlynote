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

.. _guide-stream_discard_share:

*******************************************
Internals of stream_discard_share_end_flag
*******************************************

.. toctree::
   :hidden:
   :maxdepth: 3

This document describes the structure and execution of stream_discard for streams,
implemented as :ref:`stream_discard <cid-xf::common::utils_hw::stream_discard>` function.

.. image:: /images/stream_discard/stream_discard_share.png
   :alt: discarding streams sharing end-flag structure
   :width: 80%
   :align: center

The stream_discard reads all input streams but no output.That is to say, it discards all input elements. 
This primitive includes multiple input streams and an end-flag stream, which is shared by the former.

Applicable conditions:

   1. Each input stream has the same length as others.
   2. The length of the shared end-flag stream is 1 more than the one of each input stream.



.. CAUTION::
   The Applicable conditions.

