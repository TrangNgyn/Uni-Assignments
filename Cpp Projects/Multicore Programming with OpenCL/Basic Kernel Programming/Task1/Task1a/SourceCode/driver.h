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

//Allows a user to enter a number from 1 to 100 (inclusive) and check for valid user input
//Quit program if input is invalid
bool select_number(int &input);

#endif
