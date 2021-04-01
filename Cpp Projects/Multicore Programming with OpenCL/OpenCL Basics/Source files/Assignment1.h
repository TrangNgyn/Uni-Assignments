#ifndef _SELECT_DEVICE_H_
#define _SELECT_DEVICE_H_

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


//get user input for type of device
bool select_device_type(cl_device_type &deviceType);

// load all CPU/GPU devices to a vector
void search_devices(cl_device_type deviceType, std::vector<cl::Platform> &platforms,
	std::vector<cl::Device> &all_devices, std::vector<std::pair<int, int>> &device_pltfrm);

//display all CPU/GPU devices' information
void display_devices(std::vector<cl::Platform> &platforms, std::vector<cl::Device> &devices,
	std::vector<std::pair<int, int>> &device_pltfrm);

// select an available device
bool select_device(std::vector<cl::Platform> &platforms, std::vector<cl::Device> &devices,
	std::vector<std::pair<int, int>> &device_pltfrm, cl::Platform &selectedPlat, cl::Device &selectedDev);

//return true if an extension is supported
bool check_extension_supported(cl::Device &device, std::string ext);

// output build log for all devices in context
void build_log_output(std::vector<cl::Device> &contextDevices, cl::Program &program);

//function to build a program from a source file
void build_program(cl::Program &program, cl::Context &context, std::vector<cl::Device> &contextDevices);

// create kernels and display their names
void create_kernels(cl::Program &program, std::vector<cl::Kernel> &all_kernels);

#endif