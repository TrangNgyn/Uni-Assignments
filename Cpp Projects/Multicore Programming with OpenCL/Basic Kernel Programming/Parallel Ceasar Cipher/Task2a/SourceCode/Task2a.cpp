// A C++ program to illustrate Caesar Cipher Technique 
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <vector>

#include "task2a.h"

// function to quit program
void quit_program(const std::string str)
{
	std::cout << str << std::endl;
	std::cout << "Exiting the program..." << std::endl;

#ifdef _WIN32
	// wait for a keypress on Windows OS before exiting
	std::cout << "\npress a key to quit...";
	std::cin.ignore();
#endif

	exit(1);
}

//obtain user's input for "n" and check for valid input
bool select_number(int &input) {
	std::cout << "Enter a negative or positive integer: ";

	std::string inputString;
	int enteredNumber;	// option that was selected

	std::getline(std::cin, inputString);
	std::istringstream stringStream(inputString);

	// check whether valid number entered
	// check if input was an integer
	if (stringStream >> enteredNumber)
	{
		char c;

		// check if there was anything after the integer
		if (!(stringStream >> c))
		{
			input = enteredNumber;
			return true;
		}
	}

	return false;

}


// This function receives text and shift and 
// returns the encrypted text 
std::string encrypt(std::string text, int n)
{
	std::string result = "";

	// traverse text 
	for (int i = 0; i < text.length(); i++)
	{
		if (isalpha(text.at(i))) {
			int ascii = int(fmod(double(toupper(text.at(i)) + n + 'A'), 26)) + 'A';
			result += char(ascii);
		}
		// Encrypt non-alphabetic letters 
		else {
			result += text.at(i);
		}

	}

	// Return the resulting string 
	return result;
}

// Driver program to test the above function 
int main()
{
	//prompt for user input
	int n = 0;
	if (!select_number(n)) {
		quit_program("Invalid input!");
	}
	std::cout << "Input accepted." << std::endl;
	std::cout << "\n--------------------" << std::endl;

	std::ifstream plaintextFile("plaintext.txt");
	std::ofstream cipherFile("ciphertext.txt");
	std::ofstream decryptedFile("decrypted.txt");

	std::vector<std::string> encryptedText;
	std::string line, encrypted, decrypted;

	//encrypt text and write to ciphertext.txt
	while (getline(plaintextFile, line)) {
		encrypted = encrypt(line, n);				//encrypte the line read
		encryptedText.push_back(encrypted);	//store in the vector of strings
		cipherFile << encrypted << std::endl;		//write to output file ciphertext.txt
	}
	std::cout << "Input file plaintext.txt read and text encrypted." << std::endl;
	std::cout << encryptedText.size() << " lines encrypted and stored in ciphertext.txt" << std::endl;
	std::cout << "\n--------------------" << std::endl;

	//close files
	plaintextFile.close();
	cipherFile.close();

	//decrypt text and write to decrypted.txt
	std::ifstream ciphertextIn("ciphertext.txt");
	int countDecrypted = 0;
	for (auto s : encryptedText) {
		decrypted = encrypt(s, -n);
		decryptedFile << decrypted << std::endl;
		countDecrypted++;
	}
	std::cout << countDecrypted << " lines decrypted and stored in decrypted.txt" << std::endl;
	std::cout << "\n--------------------" << std::endl;

	return 0;
#ifdef _WIN32
	// wait for a keypress on Windows OS before exiting
	std::cout << "\npress a key to quit...";
	std::cin.ignore();
#endif

	return 0;
}
