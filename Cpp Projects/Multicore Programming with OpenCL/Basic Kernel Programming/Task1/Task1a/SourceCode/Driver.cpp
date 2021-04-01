#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "common.h"
#include "driver.h"

#define LENGTH 512					//size of array in Task 1a

//Allows a user to enter a number from 1 to 100 (inclusive) and check for valid user input
//Quit program if input is invalid
bool select_number(int &input) {
	std::cout << "Enter a number between 1 and 100 (inclusive): ";

	std::string inputString;
	signed int enteredNumber;	// option that was selected

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
			// check if valid option range
			if (enteredNumber >= 1 && enteredNumber <= 100)
			{
				input = enteredNumber;
				return true;
			}
		}
	}
	// if invalid number entered
	std::cout << "\n--------------------" << std::endl;
	std::cout << "Invalid number entered." << std::endl;

	return false;

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
	std::vector<cl_int> resultArray(LENGTH);
	int inputNumber = 0;

	cl::Buffer resultBuffer;

	// initialise values
	for (int i = 0; i < LENGTH; i++)
	{
		// set resulting vector to 0s
		resultArray[i] = 0.0f;
	}

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
		if (!build_program(&program, &context, "task1a.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		// create a kernel
		kernel = cl::Kernel(program, "initArr");

		// create command queue
		queue = cl::CommandQueue(context, device);

		//====== Prompt for user input =======//
		// select a number from 1 to 100 (inclusive)
		if (!select_number(inputNumber))
		{
			// if invalid number entered
			quit_program("Invalid number entered.");
		}
		std::cout << "Input accepted." << std::endl;
		std::cout << "--------------------" << std::endl;

		// createwrite only buffer
		resultBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(cl_int) * LENGTH);

		// set kernel arguments
		kernel.setArg(0, resultBuffer);
		kernel.setArg(1, inputNumber);

		// enqueue kernel for execution
		//queue.enqueueTask(kernel);

		cl::NDRange offset(0);
		cl::NDRange globalSize(LENGTH);	// work-units per kernel

		queue.enqueueNDRangeKernel(kernel, offset, globalSize);

		std::cout << "Kernel enqueued." << std::endl;
		std::cout << "--------------------" << std::endl;

		// enqueue command to read from device to host memory
		queue.enqueueReadBuffer(resultBuffer, CL_TRUE, 0, sizeof(cl_float) * LENGTH, &resultArray[0]);

		// print the resulting array
		for (int i = 0; i < LENGTH; i++)
		{
			//formatting the result
			if (i == 0) {
				printf("[%5d, ", resultArray[i]);
			}
			else if (i == (LENGTH - 1)) {
				printf("%6d]", resultArray[i]);
			}
			else if ((i % 8) == 7) {
				printf("%6d, \n", resultArray[i]);
			}
			else {
				printf("%6d, ", resultArray[i]);
			}

		}
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
