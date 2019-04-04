#ifndef __SYNTHESIS__
#include <cstring>
#include <iostream>
#include <fstream>
#endif

#include "xf_util/axi_to_stream.h"

#define AXI_WIDTH     (64)
#define BURST_LENTH   (32)
#define DATA_NUM      (5120)
#define SCAL_AXI      (2)
const int  DDR_DEPTH   =  (DATA_NUM/SCAL_AXI);
//typedef int   		  TYPE_Strm;
typedef ap_uint<32> TYPE_Strm;
#define STRM_WIDTH     (32)
//#define TYPE_Strm     ap_uint<32>

// ------------------------------------------------------------
// top functions for aligned data
void top_align_axi_to_stream(
    ap_uint<AXI_WIDTH>* 		rbuf,
    hls::stream<TYPE_Strm >& 	ostrm,
    hls::stream<bool>& 			e_ostrm,
    const int 					num,
    const int 					offset_AXI
){
#pragma HLS INTERFACE m_axi port=rbuf       depth=DDR_DEPTH  \
		  	 offset=slave bundle=gmem_in1 	latency = 8 	\
		     num_read_outstanding = 32 \
		     max_read_burst_length = 32

#pragma HLS INTERFACE s_axilite port = rbuf   bundle=control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#ifndef __SYNTHESIS__
	if(AXI_WIDTH<8*sizeof(TYPE_Strm))
		std::cout<<"WARNING::this function is for AXI width is multiple of the align data on ddr"<<std::endl;
#endif
	xf::util::level1::axi_to_stream<AXI_WIDTH, BURST_LENTH,   TYPE_Strm >(rbuf, num, ostrm, e_ostrm ,offset_AXI );
}

// top functions for unaligned data
void top_unalign_axi_to_stream(
    ap_uint<AXI_WIDTH>* 				rbuf,
	hls::stream<TYPE_Strm >& 	ostrm,
    hls::stream<bool>& 					e_ostrm,
	const int 							len,
    const int 							offset
){
#pragma HLS INTERFACE m_axi port=rbuf       depth=DDR_DEPTH  \
		  	 offset=slave bundle=gmem_in1 	latency = 8 	\
		     num_read_outstanding = 32 \
		     max_read_burst_length = 32

#pragma HLS INTERFACE s_axilite port = rbuf   bundle=control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#ifndef __SYNTHESIS__
	if(AXI_WIDTH<8*sizeof(TYPE_Strm))
		std::cout<<"WARNING::this function is for AXI width is multiple of the align data on ddr"<<std::endl;
#endif
	xf::util::level1::axi_to_stream<AXI_WIDTH, BURST_LENTH, TYPE_Strm >(rbuf, ostrm, e_ostrm, len, offset);
}

//void top_read_to_vec(
//    ap_uint<AXI_WIDTH>* 		rbuf,
//	const int 					scal_vec,
//    const int 					num,
//	hls::stream<ap_uint<AXI_WIDTH> >& vec_strm
//){
//	xf::util::level1::details::read_to_vec<AXI_WIDTH, BURST_LENTH>(
//			rbuf, num, scal_vec,
//	      vec_strm);
//}

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
// function to align str
template <class dataType>
std::string alignStrtodataType (std::string str)
{
  // convert string into numeric value
  dataType data;
  std::stringstream strStream(str);
  strStream >> data;
  // align str
  char *bytesPtr = reinterpret_cast<char *>(&data);
  std::string alignString;
  for(unsigned i = 0; i < sizeof(dataType); i++)
	  alignString.push_back(*(bytesPtr+i));
  return alignString;
}


struct Test_Row {
  int rowIdx;
  int length;
  char *rowData;
} ;

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

	bool bin_isaligned = false;
	if (parser.getCmdOption("-isALBIN",optValue)){
		bin_isaligned = atoi(optValue.c_str());
	}else{
		std::cout << "WARNING: bin_isaligned not specified. Defaulting to " << bin_isaligned << std::endl;
	}

	bool ref_isaligned = false;
	if (parser.getCmdOption("-isALREF",optValue)){
		ref_isaligned = atoi(optValue.c_str());
	}else{
		std::cout << "WARNING: ref_isaligned not specified. Defaulting to " << ref_isaligned << std::endl;
	}

	//l_orderkey_unaligned

	//load data
	int fixedDataLen  = 4;
	int DATA_LEN_CHAR = DATA_NUM * fixedDataLen;
	char*       dataInDDR = (char*)malloc(DATA_LEN_CHAR*8*sizeof(char));
	TYPE_Strm*  rowDtmp_ap= (TYPE_Strm*)malloc(DATA_NUM*8*sizeof(TYPE_Strm));
	if (!dataInDDR){
		printf("Alloc dataInDDR failed!\n");
		return 1;
	}

	//call top
	hls::stream<TYPE_Strm > ostrm;
	hls::stream<bool>     e_ostrm;
	int err;
	const int len	 = 4799;

	const int offset = 3;
	if(bin_isaligned){
		err = load_dat<char>(dataInDDR, dataFile, in_dir, DATA_LEN_CHAR);
		if (err) return err;
		top_align_axi_to_stream((ap_uint<AXI_WIDTH>*)dataInDDR, ostrm, e_ostrm, DATA_NUM , 0);
	}else{
		err = load_dat<char>(dataInDDR, dataFile, in_dir, (len+offset+AXI_WIDTH/8-1)/(AXI_WIDTH/8)*(AXI_WIDTH/8));
		if (err) return err;
		top_unalign_axi_to_stream((ap_uint<AXI_WIDTH>*)dataInDDR, ostrm, e_ostrm, len, offset);
	}


	//strm output
    Test_Row row[DATA_NUM];
    bool e_TestRow;
    int out_num;
    if(bin_isaligned){
    	out_num = DATA_NUM;
    }else{
    	out_num = (len+STRM_WIDTH/8-1)/(STRM_WIDTH/8);
    }

    for(int i=0;i<(out_num);i++){
    	TYPE_Strm tmp;
    	ostrm.read(tmp);
        e_ostrm.read(e_TestRow);

        row[i].rowIdx  = i;
        row[i].length  = fixedDataLen;//use decodeRowLenth() instead
        *(rowDtmp_ap+i) = tmp;
        row[i].rowData = reinterpret_cast<char* >(rowDtmp_ap+i);

        if(e_TestRow){
        	std::cout << "ERROR: e_TestRow=1 while the data is not read empty! ";
        }
    }
    //read the last
    	e_ostrm.read(e_TestRow);
		if(!e_TestRow){
			std::cout << "ERROR: e_TestRow=0 while the data is read empty! ";
		}

	    // line-by-line comparison

		// Open original reference file
		std::string referFilename= dataFile.substr(0, dataFile.find(".")) + ".txt";
		std::ifstream inputFile(referFilename);
		std::cout << "Finish Reading dataFile.txt "<<std::endl;
		if (!inputFile.good()) {
			std::cout << "ERROR: Cannot open file " << dataFile << " before the compare" << std::endl;
			return 1;
		}

	    int idx=0;
	    std::string line, line_aligned;

	    while(idx < out_num){
			getline(inputFile, line);
			if(row[idx].rowIdx >= out_num) break;

			if(ref_isaligned){
				line_aligned = line;
			}else{
				line_aligned = alignStrtodataType<int>(line);
			}
			// data_lenth for specified row index


			//print the compare
			std::cout << "{ FPGA stream: ";
			for(int j=0; j<row[idx].length; j++){
			  std::cout << *(row[idx].rowData+j);
			}
			std::cout << ",  Reference: " << line_aligned <<"}"<< std::endl;

			//compare
//			if(bin_isaligned){
				if(line_aligned.compare(0,line_aligned.size(),row[idx].rowData,row[idx].length) != 0){
				  std::cout << "Failed compare!\nMismatch at Row " << row[idx].rowIdx << std::endl;
				  std::cout << "Reference: " << line_aligned << std::endl;
				  std::cout << "  FPGA stream: ";
				  for(int j=0; j<row[idx].length; j++){
					std::cout << *(row[idx].rowData+j);
				  }
				  std::cout << "\n";
				  //std::cout << "WARNING: ref_aligned not specified. Defaulting to " << ref_isaligned << std::endl;
				  return 1;
				}
				idx++;
//			}else{
//				idx++;
//			}
		}//end while
	    if(idx == out_num) std::cout << "passed compare!\n ";
	    std::cout <<"idx: "<<idx<< "= number of output data = " << out_num<< "\n";
	    inputFile.close();//added

	    free(dataInDDR);
	    free(rowDtmp_ap);
}
#endif
