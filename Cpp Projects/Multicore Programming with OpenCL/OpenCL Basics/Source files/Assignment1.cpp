#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

// functions to handle errors
#include "error.h"


// to avoid having to use prefixes
//using namespace std;
//using namespace cl;


//get user input for type of device
bool select_device_type(cl_device_type &deviceType) {
	//get user input
	std::cout << "\n===========================" << std::endl;
	std::cout << "Select a device type: 1. CPU \t2. GPU" << std::endl;
	std::cout << "Your choice: ";

	std::string inputString;
	unsigned int selectedOption;	// option that was selected

	std::getline(std::cin, inputString);
	std::istringstream stringStream(inputString);

	// check whether valid option was selected
	// check if input was an integer
	if (stringStream >> selectedOption)
	{
		char c;
		// check if there was anything after the integer
		if (!(stringStream >> c))
		{
			// check if valid option range
			if (selectedOption == 1)
			{
				deviceType = CL_DEVICE_TYPE_CPU;
				return true;
			}
			else if (selectedOption == 2) {
				deviceType = CL_DEVICE_TYPE_GPU;
				return true;
			}
		}
	}
	// if invalid option selected
	std::cout << "\n--------------------" << std::endl;
	std::cout << "Invalid option." << std::endl;

	return false;
}

// load all CPU/GPU devices to a vector
void search_devices(cl_device_type deviceType,
	std::vector<cl::Platform> &platforms,
	std::vector<cl::Device> &all_devices,
	std::vector<std::pair<int, int>> &device_pltfrm) {
	std::string outputString;
	std::vector<cl::Device> devices; //devices in each platform

	try {
		std::cout << "Searching for devices..." << std::endl;
		// get the number of available OpenCL platforms
		cl::Platform::get(&platforms);

		// for each platform
		for (unsigned int i = 0; i < platforms.size(); i++)
		{
			std::cout << "Platform " << i << ": ";
			try {
				devices.clear();
				// get all devices available to the platform
				platforms[i].getDevices(deviceType, &devices);
			}
			catch (cl::Error err) { //no device found
				//do nothing
			}
			
			std::cout << devices.size() << " device(s) found" << std::endl;

			// for each device
			for (unsigned int j = 0; j < devices.size(); j++)
			{
				//add to all_devices
				all_devices.push_back(devices[j]);
				//record the supported platform by the device
				device_pltfrm.push_back(std::make_pair(all_devices.size() - 1, i));
			}
		}
	}
	// catch any OpenCL function errors
	catch (cl::Error e) {
		// call function to handle errors
			handle_error(e);	
	}
}

//display all CPU/GPU devices' information
void display_devices(std::vector<cl::Platform> &platforms,
	std::vector<cl::Device> &devices,
	std::vector<std::pair<int, int>> &device_pltfrm) {
	std::string outputString;

	std::cout << "\n===========================" << std::endl;
	std::cout << "Available devices: " << std::endl;
	try {
		for (unsigned int i = 0; i < devices.size(); i++) {
			// device's index number
			std::cout << "\tDevice " << i << ":" << std::endl;

			// supported platform
			int p = device_pltfrm[i].second; // get supported platform number
			outputString = platforms[p].getInfo<CL_PLATFORM_NAME>();
			std::cout << "\t\tSupported platform(s): " << outputString << std::endl;

			// get and output device name
			outputString = devices[i].getInfo<CL_DEVICE_NAME>();
			std::cout << "\t\tName: " << outputString << std::endl;

			// get and output device type
			cl_device_type type;
			devices[i].getInfo(CL_DEVICE_TYPE, &type);
			if (type == CL_DEVICE_TYPE_CPU)
				std::cout << "\t\tType: " << "CPU" << std::endl;
			else if (type == CL_DEVICE_TYPE_GPU)
				std::cout << "\t\tType: " << "GPU" << std::endl;
			else
				std::cout << "\t\tType: " << "Other" << std::endl;

			//get and output compute units
			std::cout << "\t\tNumber of compute units: " << devices[i].getInfo<CL_DEVICE_MAX_COMPUTE_UNITS>() << std::endl;

			//get and output workgroup size
			std::cout << "\t\tMaximum work group size: " << devices[i].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>() << std::endl;

			//get and output workitem size
			std::vector<::size_t> maxWorkItems;
			maxWorkItems = devices[i].getInfo<CL_DEVICE_MAX_WORK_ITEM_SIZES>();
			std::cout << "\t\tMaximum work item size: " << std::endl;
			for (int k = 0; k < maxWorkItems.size(); k++) {
				std::cout << "\t\t\tFor dimension " << k << ": " << maxWorkItems[k] << std::endl;
			}

			//get and output local memory size
			std::cout << "\t\tGlobal memory size: " << devices[i].getInfo<CL_DEVICE_GLOBAL_MEM_SIZE>() << std::endl;
			std::cout << "\n--------------------" << std::endl;
		}
	}
	catch (cl::Error error) {
		handle_error(error);
	}
}

// select an available device
bool select_device(std::vector<cl::Platform> &platforms,
	std::vector<cl::Device> &devices,
	std::vector<std::pair<int, int>> &device_pltfrm,
	cl::Platform &selectedPlat, cl::Device &selectedDev) {

	try {
		std::cout << "\n===========================" << std::endl;
		std::cout << "Select a device: ";

		std::string inputString;
		unsigned int selectedOption;	// option that was selected

		std::getline(std::cin, inputString);
		std::istringstream stringStream(inputString);

		// check whether valid option was selected
		// check if input was an integer
		if (stringStream >> selectedOption)
		{
			char c;

			// check if there was anything after the integer
			if (!(stringStream >> c))
			{
				// check if valid option range
				if (selectedOption >= 0 && selectedOption < devices.size())
				{
					// return the platform and device
					int deviceNumber = device_pltfrm[selectedOption].first;
					int platformNumber = device_pltfrm[selectedOption].second;

					selectedPlat = platforms[platformNumber];
					selectedDev = devices[deviceNumber];

					std::cout << "Device " << deviceNumber << " selected" << std::endl;

					return true;
				}
			}
		}
	}
	catch (cl::Error error) {
		//handle any cl error
		handle_error(error);
	}
	// if invalid option selected
	std::cout << "\n--------------------" << std::endl;
	std::cout << "Invalid option." << std::endl;
	return false;
}

//return true if an extension is supported
bool check_extension_supported(cl::Device &device, std::string ext) {
	//space-separated list of extensions
	std::string extentions = device.getInfo<CL_DEVICE_EXTENSIONS>();

	if (extentions.find(ext) != std::string::npos) {
		std::cout << "\t" << ext << " - extension supported" << std::endl;
		return true;
	}
	else {
		std::cout << "\t" << ext << " - extension not supported" << std::endl;
		return false;
	}
}


// output build log for all devices in context
void build_log_output(std::vector<cl::Device> &contextDevices, cl::Program &program) {
	std::string outputString;

	// output build log for all devices in context
	for (unsigned int i = 0; i < contextDevices.size(); i++)
	{
		// get device name
		outputString = contextDevices[i].getInfo<CL_DEVICE_NAME>();
		std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(contextDevices[i]);

		cl_build_status status = program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(contextDevices[i]);
		std::cout << "Device - " << outputString << " - BUILD LOG:" << std::endl;
		std::cout << build_log << "\n--------------------" << std::endl;

	}




}

//function to build a program from a source file
void build_program(cl::Program &program, cl::Context &context, std::vector<cl::Device> &contextDevices) {
	std::string outputString;
	// open input file stream to .cl file
	std::ifstream programFile("source.cl");

	// check whether file was opened
	if (!programFile.is_open())
	{
		quit_program("File not found.");
	}

	// create program string and load contents from the file
	std::string programString(std::istreambuf_iterator<char>(programFile), (std::istreambuf_iterator<char>()));

	// output contents of the file
	std::cout << "\n===========================" << std::endl;
	std::cout << "Contents of program string: " << std::endl;
	std::cout << programString << std::endl;
	std::cout << "--------------------" << std::endl;

	// create program source from one input string
	cl::Program::Sources source(1, std::make_pair(programString.c_str(), programString.length() + 1));
	// create program from source
	program = cl::Program(context, source);

	// try to build program
	try {
		// build the program for the devices in the context
		program.build(contextDevices);

		std::cout << "Program build: Successful" << std::endl;
		std::cout << "--------------------" << std::endl;
		build_log_output(contextDevices, program);
	}
	catch (cl::Error e) {
		// if failed to build program
		if (e.err() == CL_BUILD_PROGRAM_FAILURE)
		{
			// output program build log
			std::cout << e.what() << ": Failed to build program." << std::endl;
			//output build log for all devices in context
			build_log_output(contextDevices, program);

		}
		else
		{
			throw e;
		}
	}
}

// create kernels and display their names
void create_kernels(cl::Program &program, std::vector<cl::Kernel> &all_kernels) {
	//get the number of kernels
	std::string kernelNames = program.getInfo<CL_PROGRAM_KERNEL_NAMES>(); //returns semicolon-separated list
	size_t count = std::count(kernelNames.begin(), kernelNames.end(), ';'); //count the number of semicolons

	std::cout << "\n===========================" << std::endl;
	//display the number of kernels
	std::cout << "There are " << (count + 1) << " kernels in the program, including:" << std::endl;
	
	//create kernels in the program
	program.createKernels(&all_kernels);
	
	//display kernel function names
	for (unsigned int i = 0; i < all_kernels.size(); i++) {
		std::string outputString = all_kernels[i].getInfo<CL_KERNEL_FUNCTION_NAME>();
		std::cout << "\tKernel " << i << ": " << outputString << std::endl;
	}
	std::cout << "--------------------" << std::endl;
}

int main(void)
{
	cl_device_type deviceType;				// type of device chosen by user
	std::vector<cl::Platform> platforms;	// available platforms
	std::vector<cl::Device> devices;		// all CPU/GPU(s) available
	std::vector<std::pair<int, int>> device_pltfrm; //device index and corresponding platform number
	
	cl::Platform selectedPlat;				// platform of the selected device
	cl::Device selectedDev;					// selected device
	cl::Context context;					// context for the selected device
	std::vector<cl::Device> contextDevices;	// devices in the created context
	cl::CommandQueue queue;					// command queue for selected device and context
	cl::Program program;					// program to be built from source file
	std::vector<cl::Kernel> all_kernels;	// all kernels created from the program source code

	
	bool validInput = select_device_type(deviceType); // check if user input is valid
	while (!validInput) {
		// re-select the device type
		validInput = select_device_type(deviceType);
	}
	std::cout << "--------------------" << std::endl;

	try {
		search_devices(deviceType, platforms, devices, device_pltfrm);
		display_devices(platforms, devices, device_pltfrm);

		// get user input to select an available device
		bool deviceSelected = select_device(platforms, devices, device_pltfrm, selectedPlat, selectedDev);

		while (!deviceSelected) { // while input is invalid
			// re-select the device
			deviceSelected = select_device(platforms, devices, device_pltfrm, selectedPlat, selectedDev);
		}
		//display the name of the selected device
		std::cout << "\tSelected device name: " << selectedDev.getInfo<CL_DEVICE_NAME>() << std::endl;

		//check if extension supported
		check_extension_supported(selectedDev, "cl_khr_icd");
		std::cout << "--------------------" << std::endl;

		// create a context from device
		context = cl::Context(selectedDev);

		// Create a command queue
		queue = cl::CommandQueue(context, selectedDev);
		
		// get devices from the context
		contextDevices = context.getInfo<CL_CONTEXT_DEVICES>();

		//read source file and build program
		build_program(program, context, contextDevices);

		//create kernels and display their names
		create_kernels(program, all_kernels);
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
