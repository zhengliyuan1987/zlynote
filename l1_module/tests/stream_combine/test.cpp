#include "xf_util/stream_combine.h"

#include "ap_int.h"
#include "hls_stream.h"

void dut(
    hls::stream<ap_uint<4> >& cfg,
    hls::stream<ap_uint<32> > in[4],
    hls::stream<bool> ein[4],
    hls::stream<ap_uint<32 * 4> >& out,
    hls::stream<bool>& eout) {
  xf::common::utils_hw::stream_combine(cfg, in, ein, out, eout, xf::common::utils_hw::combine_left_t());
}

int main(int argc, const char *argv[]) {
  int err = 0; // 0 for pass, 1 for error
  // TODO prepare cfg, in, ein; decl out, eout
  dut(cfg, in, ein, out, eout);
  // TODO check out, eout
  return err;
}
