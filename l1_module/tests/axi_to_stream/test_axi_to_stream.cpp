#include <cstring>
#include <iostream>
#include <thread>
#include <algorithm>
#include <fstream>

#include "xf_util/axi_to_stream.h"
// ------------------------------------------------------------
// load the data file (.txt, .bin, ...)to ptr
template <typename T>
int load_dat(void* data, const std::string& name, const std::string& dir,
             size_t n) {
  if (!data) {
    return -1;
  }
//  std::string fn = dir + "/" + name + ".txt";
  std::string fn =  "./" + name ;
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
  ArgParser (int &argc, char **argv){
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
// aligned_alloc in linux
#include <new>
#include <cstdlib>

template <typename T>
T* aligned_alloc(std::size_t num)
{
  void* ptr = nullptr;
  /* works on linux */
  if (posix_memalign(&ptr,4096,num*sizeof(T)))
  /* should work on windows only */
  //if (_aligned_malloc(num*sizeof(T),4096))
    throw std::bad_alloc();
  return reinterpret_cast<T*>(ptr);
}

#define AXI_WIDTH     64
#define BURST_LENTH   32
#define DATA_NUM      5120
#define SCAL_AXI      1
typedef ap_uint<32>   TYPE_Strm;

// ------------------------------------------------------------
// top functions
void top_axi_to_stream(
    ap_uint<AXI_WIDTH>* rbuf,
    hls::stream<TYPE_Strm >& ostrm,
    hls::stream<bool>& e_ostrm,
    const int num,
    const int offset = 0){
	xf::util::level1::axi_to_stream<AXI_WIDTH, BURST_LENTH, SCAL_AXI,  TYPE_Strm>(rbuf, ostrm, e_ostrm, num, offset);
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
	std::string in_dir;//no use by now

	// cmd arg parser.
	ArgParser parser(argc, argv);

	if (parser.getCmdOption("-datafile",optValue)){
		dataFile = optValue;
	}else{
		std::cout << "WARNING: data file not specified for this test. use '-datafile' to specified it. \n";
	}

	//load data
	int fixedDataLen  = 4;
	int DATA_LEN_CHAR = DATA_NUM * fixedDataLen;
	char*       dataInDDR = aligned_alloc<char  >(DATA_LEN_CHAR);
	int err;
	err = load_dat<char>(dataInDDR, "dataFile", in_dir, DATA_LEN_CHAR);
	if (err) return err;

	//call top
	hls::stream<TYPE_Strm > ostrm;
	hls::stream<bool>     e_ostrm;
	top_axi_to_stream((ap_uint<AXI_WIDTH>*)dataInDDR, ostrm, e_ostrm, DATA_NUM);

	//strm output
    Test_Row row[DATA_NUM];
    bool e_TestRow;

    for(int i=0;i<(DATA_NUM);i++){
    	TYPE_Strm      rowDtmp_ap;
    	ostrm.read(rowDtmp_ap);
        e_ostrm.read(e_TestRow);

        row[i].rowIdx  = i;
        row[i].length  = fixedDataLen;//use decodeRowLenth() instead
        row[i].rowData = reinterpret_cast<char* >(&rowDtmp_ap);

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
		std::ifstream inputFile(dataFile);
		std::cout << "Finish Reading dbfile.txt "<<std::endl;
		if (!inputFile.good()) {
			std::cout << "ERROR: Cannot open file " << dataFile << " before the compare" << std::endl;
			return 1;
		}

	    int idx=0;
	    std::string line;
	     // move to the offset from where comparison will start
	    while(getline(inputFile, line)){
	    if(row[idx].rowIdx >= DATA_NUM) break;


	    // data_lenth for specified row index


	    //print the compare
	    std::cout << "  Decoded: "<<".";
	    for(int j=0; j<row[idx].length; j++){
	      std::cout << *(row[idx].rowData+j);
	    }
	    std::cout << ",  Reference: " << line <<"."<< std::endl;

	    //compare
	    if(line.compare(0,line.size(),row[idx].rowData,row[idx].length) != 0){
	      std::cout << "Failed compare!\nMismatch at Row " << row[idx].rowIdx << std::endl;
	      std::cout << "Reference: " << line << std::endl;
	      std::cout << "  Decoded: ";
	      for(int j=0; j<row[idx].length; j++){
	        std::cout << *(row[idx].rowData+j);
	      }
	      std::cout << "\n";
	      return 1;
	    }
	    idx++;
	  }
	  std::cout <<"idx: "<<idx<< "= number of data = " << DATA_NUM<< "\n";
	  inputFile.close();//added


}
