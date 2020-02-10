#include "cache_tb.hpp"

void syn_top(hls::stream<ap_uint<BUSADDRWIDTH> >& raddrStrm,
             hls::stream<bool>& eRaddrStrm,
             hls::stream<ap_uint<BUSDATAWIDTH> >& rdataStrm,
             hls::stream<bool>& eRdataStrm,
             ap_uint<512>* ddrMem) {
    const int ddrsize = DDRSIZEIN512;
    const int uramidxw = URAMIDXWIDTH;
    const int partFactor = URAMIDXWIDTH > 12 ? 1 << (URAMIDXWIDTH - 12) : 1;
#pragma HLS INTERFACE m_axi offset = slave latency = 64 depth = ddrsize num_write_outstanding = \
    1 num_read_outstanding = 256 max_write_burst_length = 2 max_read_burst_length = 2 bundle = gmem0_0 port = ddrMem
#pragma HLS INTERFACE s_axilite port = ddrMem bundle = control

#ifndef __SYNTHESIS__
    ap_uint<64>* valid = (ap_uint<64>*)malloc((1 << (URAMIDXWIDTH - 6)) * sizeof(ap_uint<64>));
#else
    ap_uint<64> valid[1 << (URAMIDXWIDTH - 6)];

#pragma HLS ARRAY_PARTITION variable = valid block factor = partFactor dim = 1
#pragma HLS RESOURCE variable = valid core = RAM_S2P_URAM
#endif

    xf::common::utils_hw::init<ADDRWIDTH, URAMIDXWIDTH>(valid);

    xf::common::utils_hw::cache_ro<BUSADDRWIDTH, BUSDATAWIDTH, CACHELINEIDXWIDTH, ADDRURAMIDXWIDTH,
                                   ADDRIDXURAMLINEWIDTH, URAMIDXWIDTH, DATAOFFURAMIDXWIDTH, ONCHIPSTRMDEPTH>(
        raddrStrm, eRaddrStrm, rdataStrm, eRdataStrm, ddrMem, valid);
}
