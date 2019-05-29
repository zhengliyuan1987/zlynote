# Round-Robin PU Example

This example shows how to share work between multiple Process Units (PU) in axi-port, round-robin and load-balance fashion.

It asssumes that a few data in DDR will be updated by MPUs in the example.

The design of this example inludes three modules:
   1. All data are loaded from AXI port and converted to a wide width stream.
           xf::common::utils_hw::axi_to_stream<BURST_LENTH, W_AXI,ap_uint<W_STRM> >(in_buf, axi_strm, e_axi_strm, num, offset );
   2. Multiple Process Uints handle the data.
       1) dispatch data to PUs by one stream to n distribution on round robin. 
           The wide width stream are splited to n streams and each feeds a PU.
           xf::com
       2) All PUs updated data in parallel, but some work and others sleep sometimes.
          
       3) The updated data streams from PUs are merged to a wide width stream.

   3. New data stream are ouput to axi port.
           xf::common::utils_hw::stream<BURST_LENTH, W_AXI,ap_uint<W_STRM> >(in_buf, axi_strm, e_axi_strm );
   
  
   ----------------         --------------------------------------------------------         -------------------
  |                |       |                                                        |       |                   |
  |                |       |                                                        |       |                   |
  | axi to stream  |  -->  | stream to n streams   --> MPU  ---> n streams to one   |  -->  |  stream to axi    |
  |                |       |                                                        |       |                   |
  |                |       |                                                        |       |                   |
   ----------------         ---------------------------------------------------------        -------------------
  
  
  
  The detials inside update_mpu( 8 PUs) like this:
 
               split           merge
               1-->8           8-->1 
 
                 |----> PU0 ---->| 
                 |               |
                 |----> PU1 ---->|
                 |               |
                 |----> PU2 ---->|
                 |               |
                 |----> PU3 ---->|
  istrm  ----->  |               |-----> ostrm
                 |----> PU4 ---->|
                 |               |
                 |----> PU5 ---->|
                 |               |
                 |----> PU6 ---->|
                 |               |
                 |----> PU7 ---->|
 

  Assume NPU = 16.
  All PUs work in parellel at an ideal case as belows:
   PU0   ------------------------------------
   PU1   ------------------------------------
   PU2   --  --  --  --  --  --  --  --  -- 
   PU3     --  --  --  --  --  --  --  --  --
   PU4   ----    ----    ----    ----    ----
   PU5       ----    ----    ----    ----    
   PU6       ----    ----    ----    ----    
   PU7       ----    ----    ----    ----    
 
  Here, the mark(-) stands for work and blank does sleep.

