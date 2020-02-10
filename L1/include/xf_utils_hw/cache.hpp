#ifndef XF_UTILS_HW_CACHE_H
#define XF_UTILS_HW_CACHE_H

#include <hls_stream.h>
#include <ap_int.h>
#include <assert.h>

namespace xf {
namespace common {
namespace utils_hw {
namespace details {
namespace cache {

template <int BUSADDRWIDTH,         // Width of the physical bus
          int CACHELINEIDXWIDTH,    // The width to index the 512bit
          int ADDRURAMIDXWIDTH,     // The width used to index the address of URAMs
          int ADDRIDXURAMLINEWIDTH, // The width used to index the 72bit load from URAMs
          int ADDROFFURAMIDXWIDTH   // The width of DDR address, each URAM addr represent
          >
void check(hls::stream<ap_uint<BUSADDRWIDTH> >& raddrInStrm,
           hls::stream<bool>& eRaddrStrm,
           hls::stream<ap_uint<2> >& onChipStrm,
           hls::stream<ap_uint<BUSADDRWIDTH> >& raddrBothStrm,
           hls::stream<ap_uint<2> >& DDRStrm,
           hls::stream<ap_uint<BUSADDRWIDTH> >& raddrMissStrm,
           ap_uint<64>* valid) {
#pragma HLS inline off
#pragma HLS DEPENDENCE variable = valid inter false

    bool e;
    ap_uint<BUSADDRWIDTH> raddr;

#ifndef __SYNTHESIS__
    assert((1 << (6 - ADDRIDXURAMLINEWIDTH)) >= ADDROFFURAMIDXWIDTH &&
           "the max index for the 64bit value must be greater than the stored index of the off chip memory");
    std::unique_ptr<ap_uint<64>[]> onChipAddr = std::make_unique<ap_uint<64>[]>((long)(1 << ADDRURAMIDXWIDTH));
#else
    ap_uint<64> onChipAddr[(long)(1 << ADDRURAMIDXWIDTH)];
#pragma HLS DEPENDENCE variable = onChipAddr inter false
#pragma HLS RESOURCE variable = onChipAddr core = RAM_S2P_URAM
#endif

    ap_uint<64> ramData;
    ap_uint<ADDRURAMIDXWIDTH + 1> addrQue[4] = {-1, -1, -1, -1};
#pragma HLS ARRAY_PARTITION variable = addrQue complete dim = 1

    ap_uint<64> dataQue[4] = {0, 0, 0, 0};
#pragma HLS ARRAY_PARTITION variable = dataQue complete dim = 1

    ap_uint<ADDRURAMIDXWIDTH + ADDRIDXURAMLINEWIDTH - 5> validAddrQ[4] = {-1, -1, -1, -1};
#pragma HLS ARRAY_PARTITION variable = validAddrQ complete dim = 1
    ap_uint<64> validDataQ[4] = {0, 0, 0, 0};
#pragma HLS ARRAY_PARTITION variable = validDataQ complete dim = 1

    for (int i = 0; i < 4; i++) {
#pragma HLS UNROLL
        addrQue[i] = -1;
        validAddrQ[i] = -1;
        dataQue[i] = 0;
        validDataQ[i] = 0;
    }

    e = eRaddrStrm.read();

    while (!e) {
#pragma HLS PIPELINE II = 1
        ap_uint<BUSADDRWIDTH> raddr_reg = raddrInStrm.read();
        raddr = raddr_reg.range(BUSADDRWIDTH - 1, CACHELINEIDXWIDTH);
        raddrBothStrm.write(raddr_reg);

        ap_uint<ADDRURAMIDXWIDTH> addrURAMIdx =
            raddr.range(ADDRURAMIDXWIDTH - 1 + ADDROFFURAMIDXWIDTH + ADDRIDXURAMLINEWIDTH,
                        ADDROFFURAMIDXWIDTH + ADDRIDXURAMLINEWIDTH);
        ap_uint<ADDRIDXURAMLINEWIDTH> addrLineIdx =
            raddr.range(ADDRIDXURAMLINEWIDTH - 1 + ADDROFFURAMIDXWIDTH, ADDROFFURAMIDXWIDTH);
        ap_uint<ADDROFFURAMIDXWIDTH> addrOffURAMIdx = raddr.range(ADDROFFURAMIDXWIDTH - 1, 0);

        ap_uint<ADDRURAMIDXWIDTH + ADDRIDXURAMLINEWIDTH - 6> validURAMIdx =
            raddr.range(ADDRURAMIDXWIDTH + ADDRIDXURAMLINEWIDTH + ADDROFFURAMIDXWIDTH - 1, ADDROFFURAMIDXWIDTH + 6);
        ap_uint<6> validLineIdx = raddr.range(ADDROFFURAMIDXWIDTH + 5, ADDROFFURAMIDXWIDTH);

        ap_uint<64> validLine;
        if (validURAMIdx == validAddrQ[0]) {
            validLine = validDataQ[0];
        } else if (validURAMIdx == validAddrQ[1]) {
            validLine = validDataQ[1];
        } else if (validURAMIdx == validAddrQ[2]) {
            validLine = validDataQ[2];
        } else if (validURAMIdx == validAddrQ[3]) {
            validLine = validDataQ[3];
        } else {
            validLine = valid[validURAMIdx];
        }

        bool isValid = validLine[validLineIdx];

        if (addrURAMIdx == addrQue[0]) {
            ramData = dataQue[0];
        } else if (addrURAMIdx == addrQue[1]) {
            ramData = dataQue[1];
        } else if (addrURAMIdx == addrQue[2]) {
            ramData = dataQue[2];
        } else if (addrURAMIdx == addrQue[3]) {
            ramData = dataQue[3];
        } else {
            ramData = onChipAddr[addrURAMIdx];
        }

        ap_uint<1 << (6 - ADDRIDXURAMLINEWIDTH)> reg =
            ramData.range((1 << (6 - ADDRIDXURAMLINEWIDTH)) - 1 + (1 << (6 - ADDRIDXURAMLINEWIDTH)) * addrLineIdx,
                          (1 << (6 - ADDRIDXURAMLINEWIDTH)) * addrLineIdx);

        if (isValid && reg == addrOffURAMIdx) {
            onChipStrm.write(1);
            DDRStrm.write(1);
        } else {
            onChipStrm.write(0);
            DDRStrm.write(0);
        }

        raddrMissStrm.write(raddr);

        ramData.range((1 << (6 - ADDRIDXURAMLINEWIDTH)) - 1 + (1 << (6 - ADDRIDXURAMLINEWIDTH)) * addrLineIdx,
                      (1 << (6 - ADDRIDXURAMLINEWIDTH)) * addrLineIdx) = addrOffURAMIdx;
        onChipAddr[addrURAMIdx] = ramData;

        addrQue[3] = addrQue[2];
        addrQue[2] = addrQue[1];
        addrQue[1] = addrQue[0];
        addrQue[0] = addrURAMIdx;

        dataQue[3] = dataQue[2];
        dataQue[2] = dataQue[1];
        dataQue[1] = dataQue[0];
        dataQue[0] = ramData;

        validLine[validLineIdx] = 1;
        valid[validURAMIdx] = validLine;

        validAddrQ[3] = validAddrQ[2];
        validAddrQ[2] = validAddrQ[1];
        validAddrQ[1] = validAddrQ[0];
        validAddrQ[0] = validURAMIdx;

        validDataQ[3] = validDataQ[2];
        validDataQ[2] = validDataQ[1];
        validDataQ[1] = validDataQ[0];
        validDataQ[0] = validLine;

        e = eRaddrStrm.read();
    }
    DDRStrm.write(3);
    onChipStrm.write(3);
}

template <int BUSADDRWIDTH // Width of the physical bus
          >
void ctrlDDR(ap_uint<512>* ddrMem,
             hls::stream<ap_uint<2> >& DDRStrm,
             hls::stream<ap_uint<BUSADDRWIDTH> >& raddrStrm,
             hls::stream<ap_uint<512> >& rdataStrm) {
#pragma HLS inline off
    bool e;
    bool onChip;

    ap_uint<2> ctrl = DDRStrm.read();
    onChip = ctrl.get_bit(0);
    e = ctrl.get_bit(1);

    while (!e) {
#pragma HLS PIPELINE II = 1
        ap_uint<BUSADDRWIDTH> raddr = raddrStrm.read();
        if (!onChip) {
            ap_uint<512> rdata = ddrMem[raddr];
            rdataStrm.write(rdata);
        }
        ctrl = DDRStrm.read();
        onChip = ctrl.get_bit(0);
        e = ctrl.get_bit(1);
    }
}

template <int BUSADDRWIDTH,       // Width of the physical bus
          int BUSDATAWIDTH,       // Width of the physical bus
          int CACHELINEIDXWIDTH,  // The width to index the 512bit
          int DATAURAMIDXWIDTH,   // The width used to index the address of URAMs
          int DATAOFFURAMIDXWIDTH // The width of DDR address, each URAM addr represent
          >
void ctrlOnChip(hls::stream<ap_uint<2> >& onChipStrm,
                hls::stream<ap_uint<BUSADDRWIDTH> >& raddrBothStrm,
                hls::stream<ap_uint<512> >& ddrDataStrm,
                hls::stream<ap_uint<BUSDATAWIDTH> >& rdataStrm,
                hls::stream<bool>& eRdataStrm) {
#pragma HLS inline off

#ifndef __SYNTHESIS__
    assert(BUSDATAWIDTH * (1 << CACHELINEIDXWIDTH) == 512 &&
           "cache_ro_interal: Bus width * (1<<CACHELINEIDX) must be 512\n");
#endif

    ap_uint<2> ctrl;
    bool e;
    bool onChip;

    ap_uint<BUSADDRWIDTH> raddr;
    ap_uint<BUSADDRWIDTH> cacheidx;
    ap_uint<512> ddrrdata;
    ap_uint<512> onChiprdata;

    ap_uint<512> ramData;
    ap_uint<DATAURAMIDXWIDTH + 1> addrQue[4] = {-1, -1, -1, -1};
#pragma HLS ARRAY_PARTITION variable = addrQue complete dim = 1

    ap_uint<512> dataQue[4] = {0, 0, 0, 0};
#pragma HLS ARRAY_PARTITION variable = dataQue complete dim = 1

#ifndef __SYNTHESIS__
    std::unique_ptr<ap_uint<512>[]> onChipData = std::make_unique<ap_uint<512>[]>((long)(1 << DATAURAMIDXWIDTH));
#else
    ap_uint<512> onChipData[(long)(1 << DATAURAMIDXWIDTH)];
#pragma HLS DEPENDENCE variable = onChipData inter false
#pragma HLS RESOURCE variable = onChipData core = RAM_S2P_URAM
#endif

    ctrl = onChipStrm.read();
    e = ctrl.get_bit(1);
    onChip = ctrl.get_bit(0);

    while (!e) {
#pragma HLS PIPELINE II = 1

        ap_uint<BUSADDRWIDTH> raddr_reg = raddrBothStrm.read();
        raddr = raddr_reg.range(BUSADDRWIDTH - 1, CACHELINEIDXWIDTH);
        cacheidx = raddr_reg.range(CACHELINEIDXWIDTH - 1, 0);

        ap_uint<DATAURAMIDXWIDTH> addrURAMIdx =
            raddr.range(DATAURAMIDXWIDTH + DATAOFFURAMIDXWIDTH - 1, DATAOFFURAMIDXWIDTH);

        if (addrURAMIdx == addrQue[0]) {
            ramData = dataQue[0];
        } else if (addrURAMIdx == addrQue[1]) {
            ramData = dataQue[1];
        } else if (addrURAMIdx == addrQue[2]) {
            ramData = dataQue[2];
        } else if (addrURAMIdx == addrQue[3]) {
            ramData = dataQue[3];
        } else {
            ramData = onChipData[addrURAMIdx];
        }

        if (onChip) {
            onChiprdata = ramData;
            rdataStrm.write(onChiprdata.range(BUSDATAWIDTH - 1 + BUSDATAWIDTH * cacheidx, BUSDATAWIDTH * cacheidx));
        } else {
            ddrrdata = ddrDataStrm.read();
            ramData = ddrrdata;
            onChipData[addrURAMIdx] = ramData;

            addrQue[3] = addrQue[2];
            addrQue[2] = addrQue[1];
            addrQue[1] = addrQue[0];
            addrQue[0] = addrURAMIdx;

            dataQue[3] = dataQue[2];
            dataQue[2] = dataQue[1];
            dataQue[1] = dataQue[0];
            dataQue[0] = ramData;
            rdataStrm.write(ddrrdata.range(BUSDATAWIDTH - 1 + BUSDATAWIDTH * cacheidx, BUSDATAWIDTH * cacheidx));
        }

        eRdataStrm.write(false);
        ctrl = onChipStrm.read();
        e = ctrl.get_bit(1);
        onChip = ctrl.get_bit(0);
    }
    eRdataStrm.write(true);
}

template <int ADDRWIDTH,   // Actual addr width
          int URAMIDXWIDTH // The width to idex URAM
          >
void initMulti(ap_uint<64>* valid) {
    for (unsigned int i = 0; i < 4096; i++) {
#pragma HLS PIPELINE II = 1
        for (int j = 0; j < (1 << URAMIDXWIDTH) / (4096 * 64); j++) {
#pragma HLS UNROLL
            valid[j * 4096 + i] = 0;
        }
    }
}

template <int ADDRWIDTH,   // Actual addr width
          int URAMIDXWIDTH // The width to idex URAM
          >
void initSingle(ap_uint<64>* valid) {
    for (unsigned int i = 0; i < (1 << URAMIDXWIDTH) / 64; i++) {
#pragma HLS PIPELINE II = 1
        valid[i] = 0;
    }
}

} // namespace cache
} // namespace details
} // namespace utils_hw
} // namespace common
} // namespace xf

namespace xf {
namespace common {
namespace utils_hw {

/**
 * @brief initialize function for cache_ro
 *
 * This function is designed to work with cache_ro function.
 * It is used to initialize the valid array of cache_ro funcion.
 *
 * @tparam ADDRWIDTH Actual addr width
 * @tparam URAMIDXWIDTH The width to idex URAM
 *
 * @param valid The content of this array will be initialize to zero
 */
template <int ADDRWIDTH,   // Actual addr width
          int URAMIDXWIDTH // The width to idex URAM
          >
void init(ap_uint<64>* valid) {
    if (URAMIDXWIDTH - 6 > 12) {
        details::cache::initMulti<ADDRWIDTH, URAMIDXWIDTH>(valid);
    } else {
        details::cache::initSingle<ADDRWIDTH, URAMIDXWIDTH>(valid);
    }
}

/**
 * @brief cache_ro is a URAM design for caching Read-only DDR/HBM memory spaces
 *
 * This function stores history data recently loaded from DDR/HBM in the on-chip memory(URAM).
 * It aims to reduce DDR/HBM access when the memory is accessed randomly.
 *
 * @tparam BUSADDRWIDTH Width of the physical bus
 * @tparam BUSDATAWIDTH Width of the physical bus
 * @tparam CACHELINEIDXWIDTH The width to index the 512bit
 * @tparam ADDRURAMIDXWIDTH The width used to index the address of Check URAMs in 512bit
 * @tparam ADDRIDXURAMLINEWIDTH The width used to index the 72bit load from Check URAMs
 * @tparam DATAURAMIDXWIDTH The width used to index the address of DATA URAMs in 512bit
 * @tparam DATAOFFURAMIDXWIDTH The width of DDR address in 512bit, each URAM addr represent
 * @tparam ONCHIPSTRMDEPTH The strm depth for the hit addr
 *
 * @param raddrStrm The read address should be sent to this stream
 * @param eRaddrStrm The side flag to mark the end of the raddrStrm
 * @param rdataStrm The read data will be returned to this stream
 * @param eRdataStrm The side flag to mark the end of the rdataStrm
 * @param ddrMem The AXI port for the off-chip DDR/HBM memory
 * @param valid To show if the on-chip data is a valid data, use init function to initialize it. The size should be
 * 1<<(DATAURAMIDXWIDTH-6).
 */
template <int BUSADDRWIDTH,         // Width of the physical bus
          int BUSDATAWIDTH,         // Width of the physical bus
          int CACHELINEIDXWIDTH,    // The width to index the 512bit
          int ADDRURAMIDXWIDTH,     // The width used to index the address of Check URAMs in 512bit
          int ADDRIDXURAMLINEWIDTH, // The width used to index the 72bit load from Check URAMs
          int DATAURAMIDXWIDTH,     // The width used to index the address of DATA URAMs in 512bit
          int DATAOFFURAMIDXWIDTH,  // The width of DDR address in 512bit, each URAM addr represent
          int ONCHIPSTRMDEPTH       // The strm depth for the hit addr
          >
void cache_ro(hls::stream<ap_uint<BUSADDRWIDTH> >& raddrStrm,
              hls::stream<bool>& eRaddrStrm,
              hls::stream<ap_uint<BUSDATAWIDTH> >& rdataStrm,
              hls::stream<bool>& eRdataStrm,
              ap_uint<512>* ddrMem,
              ap_uint<64>* valid) {
#pragma HLS inline off
#pragma HLS dataflow
    hls::stream<ap_uint<2> > DDRStrm("cache_ro_DDRStrm");
#pragma HLS stream variable = DDRStrm depth = 32
#pragma HLS resource variable = DDRStrm core = FIFO_LUTRAM

    hls::stream<ap_uint<BUSADDRWIDTH> > addrMissStrm("cache_ro_addrMissStrm");
#pragma HLS stream variable = addrMissStrm depth = 32
#pragma HLS resource variable = addrMissStrm core = FIFO_LUTRAM

    const int depth = ONCHIPSTRMDEPTH;

    hls::stream<ap_uint<BUSADDRWIDTH> > addrBothStrm("cache_ro_addrBothStrm");
#pragma HLS stream variable = addrBothStrm depth = depth
#pragma HLS resource variable = addrBothStrm core = FIFO_BRAM

    hls::stream<ap_uint<2> > onChipStrm("cache_ro_onChipStrm");
#pragma HLS stream variable = onChipStrm depth = depth
#pragma HLS resource variable = onChipStrm core = FIFO_BRAM

    hls::stream<ap_uint<512> > ddrDataStrm("cache_ro_ddrDataStrm");
#pragma HLS stream variable = ddrDataStrm depth = 32
#pragma HLS resource variable = ddrDataStrm core = FIFO_LUTRAM

    details::cache::check<BUSADDRWIDTH, CACHELINEIDXWIDTH, ADDRURAMIDXWIDTH, ADDRIDXURAMLINEWIDTH, DATAOFFURAMIDXWIDTH>(
        raddrStrm, eRaddrStrm, onChipStrm, addrBothStrm, DDRStrm, addrMissStrm, valid);

    details::cache::ctrlDDR<BUSADDRWIDTH>(ddrMem, DDRStrm, addrMissStrm, ddrDataStrm);

    details::cache::ctrlOnChip<BUSADDRWIDTH, BUSDATAWIDTH, CACHELINEIDXWIDTH, DATAURAMIDXWIDTH, DATAOFFURAMIDXWIDTH>(
        onChipStrm, addrBothStrm, ddrDataStrm, rdataStrm, eRdataStrm);
}

} // namespace utils_hw
} // namespace common
} // namespace xf
#endif //#ifndef XF_UTILS_HW_CACHE_H
