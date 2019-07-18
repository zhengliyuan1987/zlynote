#include <vector>
#include <iostream>
#include <stdlib.h>

#include "code.hpp"

int test() {
    hls::stream<ap_uint<W_STRM> > istrm;
    hls::stream<bool> e_istrm;
    hls::stream<ap_uint<W_STRM> > ostrm;
    hls::stream<bool> e_ostrm;

    std::cout << std::dec << "W_STRM  = " << W_STRM << std::endl;
    std::cout << std::dec << "W_TAG   = " << W_TAG << std::endl;
    std::cout << std::dec << "NTAG    = " << NTAG << std::endl;
    std::cout << std::dec << "NS      = " << NS << std::endl;
    for (int d = 0; d < NS; ++d) {
        istrm.write(d);
        e_istrm.write(false);
    }
    e_istrm.write(true);

    // process
    test_core(istrm, e_istrm, ostrm, e_ostrm);

    // fetch back and check
    int nerror = 0;
    int count = 0;
    while (!e_ostrm.read()) {
        ap_uint<W_STRM> d = ostrm.read();
        ap_uint<W_STRM> gld = count + 1;
        if (count <= NS && d != gld) {
            nerror = 1;
            std::cout << "erro: "
                      << "c=" << count << ", gld=" << gld << ", "
                      << " data=" << d << std::endl;
        }
        count++;
    } // while
    std::cout << "\n total read: " << count << std::endl;
    if (count != NS) {
        nerror = 1;
        std::cout << "\n error:  total read = " << count << ", NS = " << NS << std::endl;
    }
    if (nerror) {
        std::cout << "\nFAIL: " << nerror << "the order is wrong.\n";
    } else {
        std::cout << "\nPASS: no error found.\n";
    }
    return nerror;
}

int main() {
    return test();
}
