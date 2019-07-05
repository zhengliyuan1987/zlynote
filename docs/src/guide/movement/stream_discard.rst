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

.. _guide-stream_discard:

*****************************************
Internals of stream_discard
*****************************************

.. toctree::
   :hidden:
   :maxdepth: 2

The :ref:`stream_discard <cid-xf::common::utils_hw::stream_discard>` module
works as a sink of stream(s).
It basically reads all inputs and discard them.
Users can use :ref:`stream_one_to_n <guide-stream_one_to_n>`'s ``tag_select_t``
algorithm to route data to this module to do dynamic data removal.

Three variants have been implemented, handling normal stream or synchronized
streams.

.. image:: /images/stream_discard_single.png
   :alt: discarding a singe stream structure
   :width: 80%
   :align: center

.. image:: /images/stream_discard_array.png
   :alt: discarding streams structure
   :width: 80%
   :align: center

.. image:: /images/stream_discard_share.png
   :alt: discarding streams sharing end-flag structure
   :width: 80%
   :align: center

The code implementation is a simple ``while`` loop.
