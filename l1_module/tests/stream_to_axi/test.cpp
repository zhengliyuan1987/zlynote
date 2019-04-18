
#include <ap_int.h>
#include <stdlib.h>
#include <iostream>

#include "hls_stream.h"
#include "xf_util/stream_to_axi.h"

#define WAxi (512)
#define WStrm (32)
#define NBurst (16)

// generate a random integer sequence between speified limits a and b (a<b);
int rand_int(int a, int b) { return rand() % (b - a + 1) + a; }

void stream_to_axi_test(ap_uint<WAxi> wbuf[NBurst],
                        hls::stream<ap_uint<WStrm> >& istrm,
                        hls::stream<bool>& e_strm) {
  xf::util::level1::stream_to_axi<WAxi, WStrm, NBurst>(wbuf, istrm, e_strm);
}

int main() {
  int nerror = 0;

  hls::stream<ap_uint<WStrm> > in_stream("in_stream");
  hls::stream<ap_uint<WStrm> > ref_in_stream("ref_in_stream");
  hls::stream<bool> e_strm("e_strm");

  ap_uint<WAxi> buf[NBurst];
  ap_uint<WAxi> ref_buf[NBurst];
  for (int i = 0; i < NBurst; i++) {
    buf[i] = 0;
    ref_buf[i] = rand_int(1, 32);
  }

  std::cout << "the ref data is:" << std::endl;
  for (int i = 0; i < NBurst; i++) {
    ap_uint<WAxi> total = ref_buf[i];
    std::cout << "ref_buf[" << i << "]=" << ref_buf[i] << std::endl;
    for (int j = 0; j < WAxi / WStrm; j++) {
      ap_uint<WStrm> partion = total(j * WStrm + WStrm - 1, j * WStrm);
      in_stream.write(partion);
      ref_in_stream.write(partion);
      e_strm.write(0);
    }
  }
  e_strm.write(1);
  std::cout << std::endl;
  //	for(int i=0;i<NBurst*SNum*(WAxi/WStrm);i++)
  //	{
  //		std::cout<< ref_in_stream.read() <<std::endl;
  //	}

  stream_to_axi_test(buf, in_stream, e_strm);

  for (int i = 0; i < NBurst; i++) {
    bool cmp = (buf[i] == ref_buf[i]) ? 1 : 0;
    if (!cmp) {
      nerror++;
      std::cout << "the data is incorrect." << std::endl;
    }
  }

  if (nerror) {
    std::cout << "\nFAIL: nerror= " << nerror << " errors found.\n";
  } else {
    std::cout << "\nPASS: no error found.\n";
  }

  return nerror;
}
