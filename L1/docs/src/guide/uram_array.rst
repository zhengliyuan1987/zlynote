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

.. _guide-class-uram_array:

********************************
Internals of uram_array
********************************

.. toctree::
   :hidden:
   :maxdepth: 2

This document describes the class and execution of uram_array,
implemented as :ref:`uram_array <cid-xf::common::utils_hw::uram_array>` class.


.. _my-figure-uram_array:
.. figure:: /images/uram_array.png
    :alt: bit field
    :width: 80%
    :align: center


    :ref:`uram_array workflow`

The uram_array primitive access URAM array that can be update every cycle and URAMs are 72 bit fixed. We can be read/write URAM with given width 2^n(0,1,...10) in template parameter. The uram_array provides bypass that can take out value  from cache instead of accessing URAM, and it will be improve uram_array work efficiency. 

There are Applicable conditions:

1. When _WData <= 72, there are ``_elem_per_line`` data store in the 72 bits. For example, data for 16-bit and 20k elements, need 2 URAMs.  

2. When _WData > 72, there are ``_num_parallel_block`` 72-bit in parallel blocks. For example, data for 128-bit and 10k elements, need 6 URAMs.


Profiling
=========

The hardware resources for 10k elements are listed in :numref:`tabURAM`. (vivado result)

.. _tabURAM:

.. table:: Hardware resources for URAM
    :align: center

    +-------------+----------+----------+-----------+-----------+-------------+
    |    _WData   |   URAM   |    FF    |    LUT    |  Latency  |  clock(ns)  |
    +-------------+----------+----------+-----------+-----------+-------------+
    |      64     |     3    |   3215   |   1868    |   10243   |    2.046    |
    +-------------+----------+----------+-----------+-----------+-------------+
    |      128    |     6    |   4000   |   2457    |   10243   |    2.046    |
    +-------------+----------+----------+-----------+-----------+-------------+



.. CAUTION::
   When _WData <= 36, need to determine Cosim result.


