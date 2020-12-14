#include <iostream>
#include <vector>

#include "generateValue.h"
#include "functions.h"
int main(int argc, char *argv[])
{
	if(!checkArgs(argc, argv)){
		return -1;
	}
	int seed = std::atoi(argv[2]);
	int length = std::atoi(argv[3]);
	int size = std::atoi(argv[4]);
	if (std::atoi(argv[1]) == 0){		
		int mod = std::atoi(argv[5]);
		std::vector<Codeword<Mint>> cw;		
		populateBook(cw, length, size, seed, mod);
		Codebook<Codeword<Mint>> codebook(&cw[0], size);
		codebook.Display();
		
	}else if(std::atoi(argv[1]) == 1){
		std::vector<Codeword<Melt>> cw;		
		populateBook(cw, length, size, seed);
		Codebook<Codeword<Melt>> codebook(&cw[0], size);
		codebook.Display();
	}
	return 0;
}
