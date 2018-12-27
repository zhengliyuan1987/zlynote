/**
 * @file xmem.hpp
 * Xilinx Memory Acess Library
 */

#ifndef _XMEM_H_
#define _XMEM_H_

namespace xmem {

/**
 * TODO
 *
 * @tparam SZ size of data, in byte
 * @tparam BLEN burst length
 */
template <int SZ, int BLEN = 16>
struct SeqReader {
  enum { port_width = SZ * 8; data_width = SZ * 8; buf_depth = BLEN * 2; };

  typedef ap_uint<port_width> ptype;
  typedef ap_uint<data_width> dtype;

  SeqReader(ptype* _port, hls::stream<dtype>* _strm, const int _total,
            const int _offset = 0)
      : total(_total), offset(_offset) {
#pragma HLS dataflow

    hls::stream<dtype> buf;
#pragma HLS stream variable = buf depth = buf_depth

    _read_to_buf(n, _port, buf);
    _write_to_strm(n, buf, _strm);
  }

 private:
  void _read_to_buf(const int n, ptype* port, hls::stream<ptype>& buf) {
    int start = 0;
    int end = n > total ? total : n;
    for (int i = 0; i < end; i += BLEN) {
      for (int j = 0; j < BLEN; ++j) {
#pragma HLS pipeline II = 1
        ptype t = port[offset + start + j];
        buf.write(t);
      }
      start += BLEN;
    }
    for (j = start; j < end; ++j) {
      ptype t = port[offset + start + j];
      buf.write(t);
    }
  }
  void _write_to_strm(const int n, hls::stream<ptype>& b buf,
                      hls::stream<dtype>* strm) {
    int end = n > total ? total : n;
    for (i = 0; i < end; ++i) {
#pragma HLS pipeline II = 1
      ptype t = buf.read();
      dtype o = t;
      strm->write(o);
    }
  }
  /// total number of data
  int total;
  /// offset relative to the port, in number of data.
  int offset;
}; // SeqReader

} // namespace xmem

#endif // !defined(_XMEM_H_)
