#include "cache_tb.hpp"

int main() {
    hls::stream<ap_uint<BUSADDRWIDTH> > raddrStrm("main_raddrStrm");
    hls::stream<bool> eRaddrStrm("main_eRaddrStrm");
    hls::stream<ap_uint<BUSDATAWIDTH> > rdataStrm("main_rdataStrm");
    hls::stream<bool> eRdataStrm("main_eRdataStrm");
    ap_uint<512>* ddrMem = (ap_uint<512>*)malloc(DDRSIZEIN512 * sizeof(ap_uint<512>));
    ap_uint<BUSDATAWIDTH>* ref = reinterpret_cast<ap_uint<BUSDATAWIDTH>*>(ddrMem);
    for (int i = 0; i < (1 << ADDRWIDTH); i++) {
        ref[i] = i;
    }

    int* rd = (int*)malloc(200000 * sizeof(int));
    int i;
    for (i = 0; i < 1000; i++) {
        rd[i] = (long)rand() * (long)rand() % (1 << ADDRWIDTH);
        raddrStrm.write(rd[i]);
        eRaddrStrm.write(false);
    }
    long base = (long)rand() * (long)rand() % (1 << ADDRWIDTH);
    for (; i < 2000; i++) {
        rd[i] = base;
        raddrStrm.write(rd[i]);
        eRaddrStrm.write(false);
    }
    eRaddrStrm.write(true);

    syn_top(raddrStrm, eRaddrStrm, rdataStrm, eRdataStrm, ddrMem);

    for (i = 0; i < 2000; i++) {
        int data = rdataStrm.read();
        eRdataStrm.read();
        if (data != ref[rd[i]]) {
            std::cout << "error: " << data << " " << i << " " << rd[i] << " " << ref[rd[i]] << " ddrMem is: ";
            std::cout << ddrMem[rd[i] / 8].range(511, 448) << " " << ddrMem[rd[i] / 8].range(447, 384) << " "
                      << ddrMem[rd[i] / 8].range(383, 320) << " " << ddrMem[rd[i] / 8].range(319, 256) << " ";
            std::cout << ddrMem[rd[i] / 8].range(255, 192) << " " << ddrMem[rd[i] / 8].range(191, 128) << " "
                      << ddrMem[rd[i] / 8].range(127, 64) << " " << ddrMem[rd[i] / 8].range(63, 0) << std::endl;
        }
    }
    eRdataStrm.read();

    free(ddrMem);

    return 0;
}
