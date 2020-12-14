#include<iostream>
#include <vector>
#include <cctype>
#include <algorithm>

#include "generateValue.h"
#include "functions.h"

//Mint member functions
Mint::Mint(int v, int m)
{
	value = v;
	modulus = m;
}
int Mint::getSpecialSymbol()
{
	return specialSymbol;
}
int Mint::getValue() const
{
	return value;
}
int Mint::operator-(const Mint& obj) const
{
	int result = (value - obj.value) % modulus;
	if(result >= 0){
		return result;
	}else{
		return (modulus + result);
	}
}
bool Mint::equalToZero()
{
	return (value == specialSymbol);
}
//Melt member functions
Melt::Melt(char v)
{
	value = v;
}
char Melt::getSpecialSymbol()
{
	return specialSymbol;
}
char Melt::getValue() const
{
	return value;
}
int Melt::operator-(const Melt& obj) const
{
	if(value == obj.value){
		return 0;
	}else{
		return 1;
	}
}
bool Melt::equalToZero()
{
	return (value == specialSymbol);
}

//check arguments from commandline
bool checkArgs(int argc, char *argv[])
{	
	if(argc > 1)
	{
		if(!isPositiveInt(argv[1]) || (std::atoi(argv[1]) != 0 && std::atoi(argv[1]) != 1)){
			std::cerr << "The first argument should be 0 (for Mint) or 1 (for Melt)" << std::endl;
			return false;
		}
	}
	
	if((argc != 5 && argc != 6) || (argc != 6 && std::atoi(argv[1]) == 0) || (argc != 5 && std::atoi(argv[1]) == 1)){
		std::cerr << "Invalid number of arguments" << std::endl;
		return false;
	} 
	
	for(int i=2; i<argc; i++)
	{
		if(!isPositiveInt(argv[i]) || std::atoi(argv[i]) <= 0)
		{
			std::cerr << "All agruments must be positive intergers" << std::endl;
			return false;
		}
	}
	
	return true;
}

bool isPositiveInt(char cstring[])
{
	std::string string(cstring);
	if (string.find_first_not_of("0123456789") != std::string::npos){
		return false;
	}
	return true;
}

//populate codebook
void populateBook(std::vector<Codeword<Melt>> &cw, int length, int size, int seed){
	std::vector<Melt> arr;
	for(int k=0; k < size; k++){
		arr.clear();
		for(int i=0; i< length; i++){
			if(k==0){
				int m = Melt::getSpecialSymbol();
				arr.push_back(Melt(m));
			}else{
				int m = generateMelt(seed);
				arr.push_back(Melt(m));
			}
		}
		Codeword<Melt> codeword(&arr[0], length);
		cw.push_back(codeword);
	}
}

void populateBook(std::vector<Codeword<Mint>> &cw, int length, int size, int seed, int mod){
	std::vector<Mint> arr;
	for(int k=0; k < size; k++){
		arr.clear();
		for(int i=0; i< length; i++){
			if(k==0){
				int m = Mint::getSpecialSymbol();
				arr.push_back(Mint(m, mod));
			}else{
				int m = generateMint(seed, mod);
				arr.push_back(Mint(m, mod));
			}
		}
		Codeword<Mint> codeword(&arr[0], length);
		cw.push_back(codeword);
	}
}
