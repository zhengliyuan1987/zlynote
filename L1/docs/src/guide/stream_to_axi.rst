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

.. _guide-stream_to_axi:

********************************
Internals of stream_to_axi
********************************

.. toctree::
   :hidden:
   :maxdepth: 2

This document describes the structure and execution of stream_to_axi,
implemented as :ref:`stream_to_axi <cid-xf::common::utils_hw::stream_to_axi>` function. The stream_to_axi for stream to axi master in burst. API can burst write to axi, the default burst length 32. If stream width is not enough to make a AXI port, the method would instead of 0. 

stream_to_axi workflow:
-------------


.. _my-figure-strem_to_axi:
.. figure:: /images/stream_to_axi.png
    :alt: bit field
    :width: 80%
    :align: center


    :ref:`stream to axi workflow`


Applicable conditions:

1. AXI port width should be multiple of stream width. 

This primitive performs stream_to_axi in two modules working simultaneously. 

1. countForBurst: convert stream width from _WStrm to _WAxi and count burst number.

2. burstWrite: It reads the number of burst from stream, then burst write to axi port in dataflow.

The two modules imprementation details as show follows,

.. _my-figure-param:
.. figure:: /images/burst_to_write.png
    :alt: bit field
    :width: 80%
    :align: center


    :ref:`countForBurst imprementation details`


where ``N = _WAxi/_WStrm`` , ``not enough one axi`` is the state of ``counter_for_axi < N`` and it would be instead of 0. ``not enough one burst`` is the state of ``counter_for_burst < NBurst`` and it would be as a burst to write. 

.. CAUTION::
   These Applicable conditions.

This ``stream_to_axi`` primitve has only one port for axi ptr and one port for stream output.

