#include <iostream>
#include <stdlib.h>
#include <hls_stream.h>
#include <ap_int.h>
#include <math.h>

#define DDRSIZEIN512 (5000)
#define BUSADDRWIDTH (64)
#define BUSDATAWIDTH (64)
#define ADDRWIDTH (15)    // should be smaller for CoSim 15 for example
#define URAMIDXWIDTH (11) // Should be smaller for CoSim 11 for example
#define ONCHIPSTRMDEPTH (1024)

#define CACHELINEIDXWIDTH (3) // The width to index the 512bit
#define DATAOFFURAMIDXWIDTH \
    (ADDRWIDTH - CACHELINEIDXWIDTH - URAMIDXWIDTH) // The width of DDR address in 512bit, each URAM addr represent
#define ADDRIDXURAMLINEWIDTH (6)                   // The width used to index the 72bit load from Check URAMs
#define ADDRURAMIDXWIDTH \
    (URAMIDXWIDTH - ADDRIDXURAMLINEWIDTH) // The width used to index the address of Check URAMs in 512bit

void syn_top(hls::stream<ap_uint<BUSADDRWIDTH> >& raddrStrm,
             hls::stream<bool>& eRaddrStrm,
             hls::stream<ap_uint<BUSDATAWIDTH> >& rdataStrm,
             hls::stream<bool>& eRdataStrm,
             ap_uint<512>* ddrMem);
