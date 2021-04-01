#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <random>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "common.h"

//initialise input vectors
void initVectors(std::vector<cl_int> &vec1, std::vector<cl_int> &vec2) {
	
	
	// initialise values
	for (int i = 0; i < vec1.size(); i++)
	{
		// set vec1 to random values between 10 and 20
		vec1[i] = rand() % (21 - 10) + 10;
	}

	for (int i = 0; i < vec2.size(); i++)
	{
		// set vec1 to random values between 2 and 9 for the first half
		if (i < (vec2.size() / 2)) {
			vec2[i] = rand() % (10 - 2) + 2;
		}
		else {
			// -9 to -2 for the second half
			vec2[i] = rand() % (-1 - (-9)) + (-9);
		}
	}
	
}

//print the result
void print_results(cl_int *result) {
	int linecount = 0;
	for (int i = 0; i < 4; i++)
	{
		std::printf("\n%7s %2d", "Work-item", i);
		//print v
		std::printf("\n%10s", "v: ");
		for (int a = 0; a < 8; a++) {
			std::printf("%2d ", result[linecount]);
			linecount++;
		}

		//print v1
		std::printf("\n%10s", "v1: ");
		for (int b = 0; b < 8; b++) {
			std::printf("%2d ", result[linecount]);
			linecount++;
		}

		//print v2
		std::printf("\n%10s", "v2: ");
		for (int c = 0; c < 8; c++) {
			std::printf("%2d ", result[linecount]);
			linecount++;
		}

		//print results
		std::printf("\n%10s", "results: ");
		for (int d = 0; d < 8; d++) {
			std::printf("%2d ", result[linecount]);
			linecount++;
		}

		std::printf("\n");
	}
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
	std::vector<cl_int> vec1(32);
	std::vector<cl_int> vec2(16);
	std::vector<cl_int> result(128);
	initVectors(vec1, vec2);

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
		if (!build_program(&program, &context, "task1b.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		// create a kernel
		kernel = cl::Kernel(program, "select_vec");

		// create command queue
		queue = cl::CommandQueue(context, device);



		// create buffers
		cl::Buffer vec1Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * vec1.size(), &vec1[0]);
		cl::Buffer vec2Buffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_int) * vec2.size(), &vec2[0]);
		cl::Buffer resultBuffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * result.size());

		// set kernel arguments
		kernel.setArg(0, vec1Buffer);
		kernel.setArg(1, vec2Buffer);
		kernel.setArg(2, resultBuffer);

		// enqueue kernel for execution
		cl::NDRange offset(0);
		cl::NDRange globalSize(4);

		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		std::cout << "Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(resultBuffer, CL_TRUE, 0, sizeof(cl_int) * result.size(), &result[0]);

		//display the result
		print_results(&result[0]);
		
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