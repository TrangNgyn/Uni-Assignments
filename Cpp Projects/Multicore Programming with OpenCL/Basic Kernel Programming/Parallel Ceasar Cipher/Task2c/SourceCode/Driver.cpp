#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <sstream>
#include <string>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "common.h"
#include "driver.h"

//Function for reading a file and store its contents into a vector of char
bool read_file(std::string fileName, std::vector<cl_char> &vec, unsigned int &vecLength) {

	std::ifstream inFile(fileName);
	std::string lineRead = "";
	bool successful = false;

	//read file and store to vector
	while (getline(inFile, lineRead)) {
		for (int i = 0; i < lineRead.length(); i++) {
			vec.push_back(lineRead.at(i));
			vecLength++;
			successful = true;
		}
		vec.push_back('\n');
		vecLength++;
		successful = true;
	}

	inFile.close();
	return successful;
}

bool write_file(const std::string &fileName, std::vector<cl_char> &vec) {
	std::ofstream outFile(fileName);
	bool successful = false;

	//write to file
	for (auto c : vec) {
		outFile << c;
		successful = true;
	}
	outFile.close();

	return successful;
}


//encrypt or decrypt
void encrypt_parallel(cl::Context &context, cl::Kernel &kernel, cl::CommandQueue &queue,
	int vecLen, std::vector<cl_char> &inputText, std::vector<cl_char> &resultText,
	std::string outputFile, std::vector<cl_char> &LUT) {

	//check if the text can be divided into batches of 4 characters
	int remainder = vecLen % 4;
	//if no, populate the vectors with non-alphabetic characters until condition satisfied
	//ensuring the scalar vector can be converted to a vector in kernel
	for (unsigned int i = 0; i < (4 - remainder); i++) {
		inputText.push_back('.');
	}
	resultText.resize(inputText.size());
	std::cout << (4 - remainder) << " temporary character(s) added." << std::endl;

	//create read only buffer
	cl::Buffer inputTextBuff(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * inputText.size(), &inputText[0]);
	cl::Buffer LUTBuff(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_char) * LUT.size(), &LUT[0]);
	// create write only buffer
	cl::Buffer resultBuff(context, CL_MEM_WRITE_ONLY, sizeof(cl_char) * resultText.size());

	// set kernel arguments
	kernel.setArg(0, inputTextBuff);
	kernel.setArg(1, LUTBuff);
	kernel.setArg(2, resultBuff);

	// enqueue kernel for execution
	cl::NDRange offset(0);
	cl::NDRange globalSize(inputText.size() / 4);	// work-units per kernel

	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Kernel enqueued." << std::endl;
	std::cout << "--------------------" << std::endl;

	// enqueue command to read from device to host memory
	queue.enqueueReadBuffer(resultBuff, CL_TRUE, 0, sizeof(cl_char) * resultText.size(), &resultText[0]);

	//delete dummy text at the end of the result text
	for (int i = 0; i < (4 - remainder); i++) {
		resultText.pop_back();
	}
	//write result to output file
	if (!write_file(outputFile, resultText)) {
		quit_program("File " + outputFile + " could not be opened.");
	}
	// if file is opened
	std::cout << resultText.size() << " alphabetic and non-alphabetic character(s) encrypted/decrypted." << std::endl;
	std::cout << "Write to file " << outputFile << " successfully." << std::endl;
	std::cout << "--------------------" << std::endl;
}

int main(void)
{
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device

	// declare data and memory objects
	std::vector<cl_char> inputText, resultText, resultDecrypted;
	std::vector<cl_char> LUT = {'G', 'X', 'S', 'Q', 'F', 'A', 'R', 'O', 'W', 'B', 'L', 'M', 'T',
								'H', 'C', 'V', 'P', 'N', 'Z', 'U', 'I', 'E', 'Y', 'D', 'K', 'J'};
	std::vector<cl_char> decryptedLUT(26);

	//create the reversed LUT for decrypting
	//decryptedLUT[encryptedChar - 65] = decryptedChar
	for (int i = 0; i < LUT.size(); i++) {
		int index = LUT[i] - 65;
		decryptedLUT[index] = i + 65;
	}

	//number of chars in file
	unsigned int vecLen = 0;

	//read plaintext.txt
	std::string plainFileName = "plaintext.txt";
	if (!read_file(plainFileName, inputText, vecLen)) {
		quit_program("File " + plainFileName + " could not be opened.");
	}
	// if file is opened
	std::cout << "Read file " << plainFileName << " successfully." << std::endl;
	std::cout << vecLen << " alphabetic and non-alphabetic character(s) read." << std::endl;
	std::cout << "--------------------" << std::endl;

	try {
		// select an OpenCL device
		if (!select_one_device(&platform, &device))
		{
			// if no device selected
			quit_program("Device not selected.");
		}

		// create a context from device
		context = cl::Context(device);

		// build the program
		if (!build_program(&program, &context, "task2c.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		// create a kernel
		kernel = cl::Kernel(program, "encryptLookupTable");

		// create command queue
		queue = cl::CommandQueue(context, device);
		
		//Encrypt plain text
		encrypt_parallel(context, kernel, queue, vecLen,
			inputText, resultText, "ciphertext.txt", LUT);

		//Decrypt excrypted text
		encrypt_parallel(context, kernel, queue, vecLen,
			resultText, resultDecrypted, "decrypted.txt", decryptedLUT);

	}
	// catch any OpenCL function errors
	catch (cl::Error e) {
		// call function to handle errors
		handle_error(e);
	}

#ifdef _WIN32
	// wait for a keypress on Windows OS before exiting
	std::cout << "\npress a key to quit...";
	std::cin.ignore();
#endif

	return 0;
}
