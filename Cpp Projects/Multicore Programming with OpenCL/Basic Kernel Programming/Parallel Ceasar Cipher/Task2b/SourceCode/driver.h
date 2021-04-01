#pragma once
#ifndef _DRIVER_H_
#define _DRIVER_H_

#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

//Function for reading a file and store its contents into a vector of char
bool read_file(std::string fileName, std::vector<cl_char> &vec, unsigned int &vecLength);

//Write o a file using input vector
bool write_file(const std::string &fileName, std::vector<cl_char> &vec);

//encrypt or decrypt
void encrypt_parallel(cl::Context &context, cl::Kernel kernel, cl::CommandQueue queue,
	int vecLen, std::vector<cl_char> &inputText, std::vector<cl_char> &resultText,
	std::string outputFile, int inputNumber);

#endif
