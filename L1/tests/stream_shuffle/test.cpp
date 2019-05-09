#include "xf_utils_hw/stream_shuffle.h"
#include <iostream>

#define NUM_INPUT 16
#define NUM_OUTPUT 14

#define STRM_LEN 10

#define DATA_TYPE int

int nerror;

void gld(ap_int<8> gld_cfg[NUM_INPUT],
         DATA_TYPE gld_input[NUM_INPUT][STRM_LEN],
         DATA_TYPE gld_output[NUM_OUTPUT][STRM_LEN]) {
  for (int i = 0; i < NUM_OUTPUT; i++) {
    for (int j = 0; j < STRM_LEN; j++) {
      if (gld_cfg >= 0)
        gld_output[gld_cfg[i]][j] = gld_input[i][j];
      else if (gld_cfg[i][j] != -1) {
        std::cout << "error: illegal config value" << std::endl;
        nerror++;
      }
    }
  }
}

void top(hls::stream<ap_int<8> > order_cfg[NUM_INPUT],

         hls::stream<DATA_TYPE> istrms[NUM_INPUT],
         hls::stream<bool>& e_istrm,

         hls::stream<DATA_TYPE> ostrms[NUM_OUTPUT],
         hls::stream<bool>& e_ostrm) {
  xf::common::utils_hw::stream_shuffle<DATA_TYPE, NUM_INPUT, NUM_OUTPUT>(
      order_cfg, istrms, e_istrm, ostrms, e_ostrm);
}

int main() {
  nerror = 0;
  hls::stream<ap_int<8> > order_cfg[NUM_INPUT];

  hls::stream<DATA_TYPE> istrms[NUM_INPUT];
  hls::stream<bool> e_istrm;

  hls::stream<DATA_TYPE> ostrms[NUM_INPUT];
  hls::stream<bool> e_ostrm;

  ap_int<8> gld_cfg[NUM_INPUT];
  DATA_TYPE gld_input[NUM_INPUT][STRM_LEN];
  DATA_TYPE gld_output[NUM_OUTPUT][STRM_LEN];

  int i;
  for (i = 0; i < NUM_OUTPUT; i++) {
    order_cfg[i].write(i);
    gld_cfg[i] = i;
  }

  for (; i < NUM_INPUT; i++) {
    order_cfg[i].write(-1);
    gld_cfg[i] = -1;
  }

  for (int j = 0; j < STRM_LEN; j++) {
    for (int i = 0; i < NUM_INPUT; i++) {
      istrms[i].write(i);
      gld_input[i][j] = i;
    }
    e_istrm.write(false);
  }
  e_istrm.write(true);

  gld(gld_cfg, gld_input, gld_output);

  top(order_cfg, istrms, e_istrm, ostrms, e_ostrm);

  DATA_TYPE test_data;
  bool rd_success = 0;
  bool e;

  e_ostrm.read();
  for (int i = 0; i < NUM_OUTPUT; i++) {
    for (int j = 0; j < STRM_LEN; j++) {
      rd_success = ostrms[i].read_nb(test_data);
      if (test_data != gld_output[i][j]) {
        nerror++;
        std::cout << "error: test data = " << test_data
                  << " gold data = " << gld_output[i][j] << std::endl;
      }
      if (!rd_success) {
        nerror++;
        std::cout << "error: data loss" << std::endl;
      }
    }
  }

  for (int j = 0; j < STRM_LEN; j++) {
    rd_success = e_ostrm.read_nb(e);
    if (!rd_success) {
      nerror++;
      std::cout << "error: end flag loss" << std::endl;
    }
  }

  if (nerror) {
    std::cout << "\nFAIL: " << nerror;
  } else {
    std::cout << "\nPASS: no error found.\n";
  }

  return nerror;
}
