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

//initialise input vectors
void initVectors(std::vector<cl_int> &vec1, std::vector<cl_int> &vec2);

//print the result
void print_results(cl_int *result);

#endif
