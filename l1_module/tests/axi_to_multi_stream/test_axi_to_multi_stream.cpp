#ifndef __SYNTHESIS__
#include <cstring>
#include <iostream>
#include <fstream>
#endif

#include "xf_util/axi_to_multi_stream.h"

#define AXI_WIDTH     (512)
#define BURST_LENTH   (32)
//#define DATA_NUM      (5120)
#define DATA_NUM      (500000)
#define SCAL_AXI      (1)

//Simulation of data on DDR, form the DDT ptr
#define STRM_WIDTH     (32)
typedef ap_uint<STRM_WIDTH>    TYPE_Strm;

//for input
#define STRM_WIDTH0     (32)
#define STRM_WIDTH1     (32)
#define STRM_WIDTH2     (64)
typedef ap_uint<STRM_WIDTH0>    TYPE_Strm0;
typedef ap_uint<STRM_WIDTH1>    TYPE_Strm1;
typedef ap_uint<STRM_WIDTH2>    TYPE_Strm2;
const int  DDR_DEPTH   =  (DATA_NUM/SCAL_AXI);


// ------------------------------------------------------------
struct Test_Row {
  int rowIdx;
  int length;
  char *rowData;
} ;

// ------------------------------------------------------------
// function to read strm and print ii=4
template< typename _TStrm>
void readbatchToPrintII1(
	    hls::stream<_TStrm >& ostrm,
	    hls::stream<bool>& e_ostrm,
		_TStrm* rowDtmp_ap,
		const int num
){

	_TStrm dat=0;
	bool e_TestRow=0;

    for(int i=0;i<(num);i++){
#pragma HLS PIPELINE II=1

		ostrm.read(dat);
		e_ostrm.read(e_TestRow);
		*(rowDtmp_ap+i) = dat;

#ifndef __SYNTHESIS__
        if(e_TestRow){
        	std::cout << "ERROR: e_TestRow=1 while the data is not read empty!! "<<std::endl;
        }
#endif

    }
    e_ostrm.read(e_TestRow);
#ifndef __SYNTHESIS__
	if(!e_TestRow){
		std::cout << "ERROR: II =1 e_TestRow=0 while the data is read empty!! "<<std::endl;
	}
#endif

}
// function to read strm and print ii=2
template< typename _TStrm>
void readbatchToPrintII2(
	    hls::stream<_TStrm >& ostrm,
	    hls::stream<bool>& e_ostrm,
		//_TStrm* rowDtmp_ap,
		 hls::stream<_TStrm >& r_strm,
		const int num
){

	_TStrm dat;
	bool e_TestRow;

    for(int i=0;i<(2*num);i++){
#pragma HLS PIPELINE II=1

    	if((i&1)!=1){
    		//*(rowDtmp_ap+i/2+1)= 0;
    	}else{
    	ostrm.read(dat);
        e_ostrm.read(e_TestRow);
        //*(rowDtmp_ap+i/2) = dat;
        r_strm.write(dat);

#ifndef __SYNTHESIS__
        if(e_TestRow){
        	std::cout << "ERROR: II =2 e_TestRow=1 while the data is not read empty!! "<<std::endl;
        }
#endif
    	}
    }
    e_ostrm.read(e_TestRow);
#ifndef __SYNTHESIS__
	if(!e_TestRow){
		std::cout << "ERROR: e_TestRow=0 while the data is read empty!! "<<std::endl;
	}
#endif

}
// function to read strm and print ii=8
template< typename _TStrm>
void readbatchToPrintII8(
	    hls::stream<_TStrm >& ostrm,
	    hls::stream<bool>& e_ostrm,
		_TStrm* rowDtmp_ap,
		const int num
){

	_TStrm dat;
	bool e_TestRow;

    for(int i=0;i<(8*num);i++){
#pragma HLS PIPELINE II=1

    	if((i&7)!=7){
    		*(rowDtmp_ap+i/8+1)= 0;
    	}else{
    	ostrm.read(dat);
        e_ostrm.read(e_TestRow);
        *(rowDtmp_ap+i/8) = dat;

#ifndef __SYNTHESIS__
 #if 0
        Test_Row row;
        row.rowIdx  = i;
        row.length  = 8;
        row.rowData = reinterpret_cast<char* >(rowDtmp_ap+i/8);

		//print
        uint64_t *tmp=reinterpret_cast<uint64_t* >(rowDtmp_ap+i/8);
		std::cout << "{ FPGA stream2: ";
		std::cout << std::hex<<*tmp;
		std::cout <<"}"<<std::endl;
 #endif
        if(e_TestRow){
        	std::cout << "ERROR: e_TestRow=1 while the data is not read empty!! "<<std::endl;
        }
#endif
    	}
    }
    e_ostrm.read(e_TestRow);
#ifndef __SYNTHESIS__
	if(!e_TestRow){
		std::cout << "ERROR: II =8 e_TestRow=0 while the data is read empty!! "<<std::endl;
	}
#endif
}

// ------------------------------------------------------------
#if 0
// top functions for 3 type data
void top_axi_to_multi_stream(
		    ap_uint<AXI_WIDTH>* rbuf,
		    hls::stream<TYPE_Strm0 >& ostrm0,
		    hls::stream<bool>& e_ostrm0,
		    hls::stream<TYPE_Strm1 >& ostrm1,
		    hls::stream<bool>& e_ostrm1,
		    hls::stream<TYPE_Strm2 >& ostrm2,
		    hls::stream<bool>& e_ostrm2,
			const int len[3],
		    const int offset[3]
){
#pragma HLS INTERFACE m_axi port=rbuf       depth=DDR_DEPTH  \
		  	 offset=slave bundle=gmem_in1 	latency = 8 	\
		     num_read_outstanding = 32 \
		     max_read_burst_length = 32

#pragma HLS INTERFACE s_axilite port = rbuf   bundle=control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#ifndef __SYNTHESIS__
	if(len[0]<=0 ||(len[1]<=0 )|| (len[2]<=0 ) )
		std::cout<<"ERROR: len<= 0, testcase can not work!"<<std::endl;
#endif
	xf::util::level1::axi_to_multi_stream<AXI_WIDTH, BURST_LENTH, TYPE_Strm0, TYPE_Strm1, TYPE_Strm2 >
	(rbuf, ostrm0, e_ostrm0, ostrm1, e_ostrm1, ostrm2, e_ostrm2, len, offset);

}
#endif
// ------------------------------------------------------------

// top functions for co-sim
void top_for_co_sim(
		    ap_uint<AXI_WIDTH>* rbuf,
			const int 	len[3],
		    const int 	offset[3],
//			TYPE_Strm0 rowDtmp_ap0[DATA_NUM],
//			TYPE_Strm1 rowDtmp_ap1[DATA_NUM],
//			TYPE_Strm2 rowDtmp_ap2[DATA_NUM],
			hls::stream<TYPE_Strm0 >& r_strm0,
			hls::stream<TYPE_Strm1 >& r_strm1,
			hls::stream<TYPE_Strm2 >& r_strm2,
			const int 	num0,
			const int 	num1,
			const int 	num2
){
#pragma HLS DATAFLOW
	#pragma HLS INTERFACE m_axi port=rbuf       depth=DDR_DEPTH  \
			  	 offset=slave bundle=gmem_in1 	latency = 8 	\
			     num_read_outstanding = 32 \
			     max_read_burst_length = 32

	#pragma HLS INTERFACE s_axilite port = rbuf   bundle=control
	#pragma HLS INTERFACE s_axilite port = return bundle = control


#pragma HLS ARRAY_PARTITION    variable=len
#pragma HLS ARRAY_PARTITION    variable=offset
//#pragma HLS ARRAY_PARTITION    variable=num

	hls::stream<bool>      e_ostrm0;
    hls::stream<TYPE_Strm0 > ostrm0;
    hls::stream<TYPE_Strm1 > ostrm1;
    hls::stream<bool>      e_ostrm1;
    hls::stream<TYPE_Strm2 > ostrm2;
    hls::stream<bool>      e_ostrm2;

#pragma HLS RESOURCE variable= ostrm0   core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= ostrm0   depth = NONBLOCK_DEPTH
#pragma HLS RESOURCE variable= e_ostrm0 core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= e_ostrm0 depth = NONBLOCK_DEPTH
#pragma HLS RESOURCE variable= ostrm1   core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= ostrm1   depth = NONBLOCK_DEPTH
#pragma HLS RESOURCE variable= e_ostrm1 core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= e_ostrm1 depth = NONBLOCK_DEPTH
#pragma HLS RESOURCE variable= ostrm2   core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= ostrm2   depth = NONBLOCK_DEPTH
#pragma HLS RESOURCE variable= e_ostrm2 core  = FIFO_LUTRAM
#pragma HLS STREAM   variable= e_ostrm2 depth = NONBLOCK_DEPTHS

	xf::util::level1::axi_to_multi_stream<AXI_WIDTH, BURST_LENTH, TYPE_Strm0, TYPE_Strm1, TYPE_Strm2 >
	(rbuf, ostrm0, e_ostrm0, ostrm1, e_ostrm1, ostrm2, e_ostrm2, len, offset);
//	readbatchToPrintII2(ostrm0, e_ostrm0, rowDtmp_ap0,  num0 );
//	readbatchToPrintII2(ostrm1, e_ostrm1, rowDtmp_ap1,  num1 );
//	readbatchToPrintII2(ostrm2, e_ostrm2, rowDtmp_ap2,  num2 );
	readbatchToPrintII2(ostrm0, e_ostrm0, r_strm0,  num0 );
	readbatchToPrintII2(ostrm1, e_ostrm1, r_strm1,  num1 );
	readbatchToPrintII2(ostrm2, e_ostrm2, r_strm2,  num2 );
}


#ifndef __SYNTHESIS__
// ------------------------------------------------------------
// load the data file (.txt, .bin, ...)to ptr
template <typename T>
int load_dat(void* data, const std::string& name, const std::string& dir,
             size_t n) {
  if (!data) {
    return -1;
  }
//  std::string fn = dir + "/" + name + ".txt";
  std::string fn =   name ;
  FILE* f = fopen(fn.c_str(), "rb");
  std::cout << "WARNING: " << fn << " will be opened for binary read."
                << std::endl;
  if (!f) {
    std::cerr << "ERROR: " << fn << " cannot be opened for binary read."
              << std::endl;
  }
  void * data_tmp;
  size_t cnt = fread(data, sizeof(T), n, f);
  fclose(f);
  if (cnt != n) {
    std::cerr << "ERROR: " << cnt << " entries read from " << fn << ", " << n
              << " entries required." << std::endl;
    return -1;
  }
  return 0;
}

// ------------------------------------------------------------
// get the arg
#include <algorithm>
#include <string>
#include <vector>
class ArgParser{
public:
  ArgParser (int &argc, const char*argv[]){
    for (int i=1; i < argc; ++i)
      mTokens.push_back(std::string(argv[i]));
  }
  bool getCmdOption(const std::string option, std::string& value) const{
    std::vector<std::string>::const_iterator itr;
    itr =  std::find(this->mTokens.begin(), this->mTokens.end(), option);
    if (itr != this->mTokens.end() && ++itr != this->mTokens.end()){
      value = *itr;
      return true;
    }
    return false;
  }
  bool getCmdOption(const std::string option) const{
    std::vector<std::string>::const_iterator itr;
    itr =  std::find(this->mTokens.begin(), this->mTokens.end(), option);
    if (itr != this->mTokens.end()) return true;
    else return false;
  }
private:
  std::vector <std::string> mTokens;
};

// ------------------------------------------------------------
// function to print
template< typename _TStrm>
void PrintRowFile(
	_TStrm* rowDtmp_ap,
	int& num
){
	std::ofstream fout("file.dat" , std::ios::app );
	for(int i=0;i<(num);i++){
		char* rowData = reinterpret_cast<char* >(rowDtmp_ap+i);
		for(int j=0; j<sizeof(_TStrm); j++){
			fout << *(rowData+j);
		}
	}
	fout.close();
}
template< typename _TStrm>
void PrintStrmRowFile(
	hls::stream<_TStrm >& r_strm,
	int& num
){
	std::ofstream fout("file.dat" , std::ios::app );
	for(int i=0;i<(num);i++){
		_TStrm tmp = r_strm.read();
		char* rowData = reinterpret_cast<char* >(&tmp);
		for(int j=0; j<sizeof(_TStrm); j++){
			fout << *(rowData+j);
		}
	}
	fout.close();
}

int main(int argc, const char* argv[]) {

	std::cout << "\n------------ Test for axi_to_stream  -------------\n";
	std::string optValue;
	std::string dataFile;
	std::string in_dir  ="./";//no use by now

	// cmd arg parser.
	ArgParser parser(argc, argv);

	if (parser.getCmdOption("-dataFile",optValue)){
		dataFile = optValue;
	}else{
		std::cout << "WARNING: data file not specified for this test. use '-datafile' to specified it. \n";
	}

	bool offset_iszero = true;
	if (parser.getCmdOption("-isZERO",optValue)){
		offset_iszero = atoi(optValue.c_str());
	}else{
		std::cout << "WARNING: offset_iszero not specified. Defaulting to " << offset_iszero << std::endl;
	}

	//load data
	//int fixedDataLen  = 4;
	const int   DATA_LEN_CHAR = DATA_NUM * 12;
	char* dataInDDR = (char*)malloc(DATA_LEN_CHAR*8*sizeof(char));
	//TYPE_Strm*  rowDtmp_ap= (TYPE_Strm*)malloc(DATA_NUM*8*sizeof(TYPE_Strm));
//	TYPE_Strm0*  rowDtmp_ap0= (TYPE_Strm0*)malloc(DATA_NUM*8*sizeof(TYPE_Strm0));
//	TYPE_Strm1*  rowDtmp_ap1= (TYPE_Strm1*)malloc(DATA_NUM*8*sizeof(TYPE_Strm1));
//	TYPE_Strm2*  rowDtmp_ap2= (TYPE_Strm2*)malloc(DATA_NUM*8*sizeof(TYPE_Strm2));
	if (!dataInDDR){
		printf("Alloc dataInDDR failed!\n");
		return 1;
	}

	//call top
	int err;
//	int len[3]	 = {4799, 5092, 7040};//{4799, 1273, 5120};
//	int offset[3]= {0, 4800,  9892};
	int len[3]	 = {721747, 499696, 1062500};//{4799, 124924, 500000};
	int offset[3]= {0, 721748,  1221444};


	int len_all = len[0]+1+len[1]+len[2];
	//err = load_dat<char>(dataInDDR, dataFile, in_dir, (len_all+offset[0]+AXI_WIDTH/8-1)/(AXI_WIDTH/8)*(AXI_WIDTH/8));
	err = load_dat<char>(dataInDDR, dataFile, in_dir, (len_all));
	if (err) return err;


	//strm output
    int out_num[3];

    out_num[0] = (len[0]+STRM_WIDTH0/8-1)/(STRM_WIDTH0/8);
    out_num[1] = (len[1]+STRM_WIDTH1/8-1)/(STRM_WIDTH1/8);
    out_num[2] = (len[2]+STRM_WIDTH2/8-1)/(STRM_WIDTH2/8);

//	TYPE_Strm0 rowDtmp_ap0[DATA_NUM];
//	TYPE_Strm1 rowDtmp_ap1[DATA_NUM];
//	TYPE_Strm2 rowDtmp_ap2[DATA_NUM];
	hls::stream<TYPE_Strm0 > r_strm0;
	hls::stream<TYPE_Strm1 > r_strm1;
	hls::stream<TYPE_Strm2 > r_strm2;

    top_for_co_sim((ap_uint<AXI_WIDTH>*)dataInDDR,len, offset,
    				//rowDtmp_ap0, rowDtmp_ap1, rowDtmp_ap2,
    				r_strm0,r_strm1,r_strm2,
					out_num[0],out_num[1],out_num[2]);

//    printf("**************************\n");
//    printf("Read %d strm0:\n", out_num[0]);
//    PrintRowFile(rowDtmp_ap0,out_num[0]);
//    printf("**************************\n");
//    printf("Read %d strm1:\n", out_num[1]);
//    PrintRowFile(rowDtmp_ap1,out_num[1]);
//    printf("**************************\n");
//    printf("Read %d strm2:\n", out_num[2]);
//    PrintRowFile(rowDtmp_ap2,out_num[2]);
    printf("**************************\n");
    printf("Read %d strm0:\n", out_num[0]);
    PrintStrmRowFile(r_strm0, out_num[0]);
    printf("**************************\n");
    printf("Read %d strm1:\n", out_num[1]);
    PrintStrmRowFile(r_strm1, out_num[1]);
    printf("**************************\n");
    printf("Read %d strm2:\n", out_num[2]);
    PrintStrmRowFile(r_strm2, out_num[2]);
    printf("Read all %d vec\n", (out_num[0]+out_num[1]+out_num[2]));

//**************
// vim compare
// csim:  vimdiff l_orderkey_veint.bin ./prj_axi_to_multi_stream/solution1/csim/build/file.dat
// cosim: vimdiff l_orderkey_veint.bin ./prj_axi_to_multi_stream/solution1/sim/wrapc/file.dat
// If all characters are the same before the "0a" character in the "file.dat", the test passed!
//**************

	free(dataInDDR);
//	free(rowDtmp_ap0);
//	free(rowDtmp_ap1);
//	free(rowDtmp_ap2);
}
#endif
