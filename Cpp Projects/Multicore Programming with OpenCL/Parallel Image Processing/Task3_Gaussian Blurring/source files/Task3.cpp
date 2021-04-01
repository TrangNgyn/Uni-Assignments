#define CL_USE_DEPRECATED_OPENCL_2_0_APIS	// using OpenCL 1.2, some functions deprecated in OpenCL 2.0
#define __CL_ENABLE_EXCEPTIONS				// enable OpenCL exemptions

// C++ standard library and STL headers
#include <iostream>
#include <vector>
#include <fstream>

// OpenCL header, depending on OS
#ifdef __APPLE__
#include <OpenCL/cl.hpp>
#else
#include <CL/cl.hpp>
#endif

#include "common.h"
#include "bmpfuncs.h"

#define NUM_ITERATIONS 1000

// forward declartions
void task3a (cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
			unsigned char *inputImage, unsigned char *outputImage, int imgWidth, int imgHeight);
void task3b (cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
			unsigned char *inputImage, unsigned char *outputImage, unsigned char *outputImage2, int imgWidth, int imgHeight);
void task3c (cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
			unsigned char *inputImage, unsigned char *outputImage, int imgWidth, int imgHeight);

int main (void) {
	cl::Platform platform;		// device platfom
	cl::Device device;			// device chosen
	cl::Context context;		// context for device
	cl::Program program;		// OpenCL program 
	cl::CommandQueue queue;		// comandqueue for context and device

	// declare data and memory objects
	unsigned char* inputImage;
	unsigned char* outputImage;
	unsigned char* outputImage2;
	int imgWidth, imgHeight, imageSize;
		

	try	{
		// select an OpenCl device
		if (!select_one_device(&platform, &device))
			quit_program("Device not Selected");

		context = cl::Context(device);

		if (!build_program(&program, &context, "image_filter.cl"))
			quit_program("OpenCl program build error");

		queue = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE);

		// read input image
		inputImage = read_BMP_RGB_to_RGBA("peppers.bmp", &imgWidth, &imgHeight);

		// allocate resources
		imageSize = imgWidth * imgHeight * 4;
		outputImage = new unsigned char[imageSize];
		outputImage2 = new unsigned char[imageSize];

		// Run tasks
		task3a(queue, program, context, inputImage, outputImage, imgWidth, imgHeight);
		task3b (queue, program, context, inputImage, outputImage, outputImage2, imgWidth, imgHeight);
		task3c (queue, program, context, inputImage, outputImage, imgWidth, imgHeight);
		
		// Free resources 
		free(inputImage);
		free(outputImage);
		free(outputImage2);

	} catch (cl::Error e) {
		handle_error(e);
	}

#ifdef _WIN32
	// wait for a keypress on Windows OS before exiting
	std::cout << "\npress a key to quit...";
	std::cin.ignore();
#endif

	return 0;
}

void task3a (cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
			unsigned char *inputImage, unsigned char *outputImage, int imgWidth, int imgHeight) {
	
	cl::ImageFormat imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
	cl::Event profileEvent;
	cl_ulong timeStart, timeEnd, timeTotal;

	cl::Kernel kernel = cl::Kernel(program, "filter");
	cl::Image2D inputImgBuffer = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)inputImage);
	cl::Image2D outputImgBuffer = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImage);

	kernel.setArg(0, inputImgBuffer);
	kernel.setArg(1, outputImgBuffer);

	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(imgWidth, imgHeight);
	timeTotal = 0;
	
	// Enqueue 1000 times and calculate average run time
	for (int i = 0; i < NUM_ITERATIONS; i++)
	{
		queue.enqueueNDRangeKernel(kernel, offset, globalSize, cl::NullRange, NULL, &profileEvent);
		queue.finish();

		timeStart = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		timeEnd = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();
		timeTotal += timeEnd - timeStart;
	}
	std::cout << "Task A executed. Kernel enqueued." << std::endl;

	// read image from buffer
	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = imgWidth;
	region[1] = imgHeight;
	region[2] = 1;

	queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImage);
	write_BMP_RGBA_to_RGB("Task3a.bmp", outputImage, imgWidth, imgHeight);

	printf("Average run time = %lu\n", timeTotal / NUM_ITERATIONS);

	std::cout << "--------------------" << std::endl;
}

void task3b (cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
	unsigned char *inputImage, unsigned char *outputImage, unsigned char *outputImage2, int imgWidth, int imgHeight) {
	
	cl::ImageFormat imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
	cl::Event profileEvent;
	cl_ulong timeStart, timeEnd, timeTotal;

	// first pass (Horizontal pass)
	cl::Kernel kernel = cl::Kernel(program, "filter_two_passes");
	
	// create buffer objects
	cl::Image2D inputImgBuffer = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)inputImage);
	cl::Image2D outputImgBuffer = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImage);

	//set kernel arguments
	kernel.setArg(0, inputImgBuffer);
	kernel.setArg(1, 0);
	kernel.setArg(2, outputImgBuffer);
	timeTotal = 0;

	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(imgWidth, imgHeight);
	timeTotal = 0;

	//enqueue 1000 times and calculate the average run time
	for (int i = 0; i < NUM_ITERATIONS; i++)
	{
		queue.enqueueNDRangeKernel(kernel, offset, globalSize, cl::NullRange, NULL, &profileEvent);
		queue.finish();

		timeStart = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		timeEnd = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();
		timeTotal += timeEnd - timeStart;
	}
	std::cout << "Task B executed.\n Kernel enqueued for first pass." << std::endl;

	//read image from output buffer
	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = imgWidth;
	region[1] = imgHeight;
	region[2] = 1;

	queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

	// second pass
	inputImgBuffer = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*) outputImage);
	outputImgBuffer = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImage2);

	//set kernel arguments
	kernel.setArg(0, inputImgBuffer);
	kernel.setArg(1, 2);
	kernel.setArg(2, outputImgBuffer);

	//enqueue 1000 times and calculate the average run time
	for (int i = 0; i < NUM_ITERATIONS; i++)
	{
		queue.enqueueNDRangeKernel(kernel, offset, globalSize, cl::NullRange, NULL, &profileEvent);
		queue.finish();

		timeStart = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		timeEnd = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();
		timeTotal += timeEnd - timeStart;
	}
	std::cout << " Kernel enqueued for second pass." << std::endl;

	printf("Average run time = %lu\n", timeTotal / NUM_ITERATIONS);
	std::cout << "--------------------" << std::endl;

	queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImage2);

	write_BMP_RGBA_to_RGB("Task3b.bmp", outputImage2, imgWidth, imgHeight);
}

void task3c (cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
	unsigned char *inputImage, unsigned char *outputImage, int imgWidth, int imgHeight) {

	cl::ImageFormat imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
	cl::Event profileEvent;
	cl_ulong timeStart, timeEnd, timeTotal;

	cl::Kernel kernel = cl::Kernel(program, "filter_1D");

	// create buffer objects
	cl::Image2D inputImgBuffer = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)inputImage);
	cl::Image2D outputImgBuffer = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImage);

	// set kernel arguments
	kernel.setArg(0, inputImgBuffer);
	kernel.setArg(1, outputImgBuffer);
	timeTotal = 0;

	cl::NDRange offset(0);
	cl::NDRange globalSize(imgHeight*imgWidth);

	//enqueue 1000 times and calculate the average run time
	for (int i = 0; i < NUM_ITERATIONS; i++)
	{
		queue.enqueueNDRangeKernel(kernel, offset, globalSize, cl::NullRange, NULL, &profileEvent);
		queue.finish();

		timeStart = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_START>();
		timeEnd = profileEvent.getProfilingInfo<CL_PROFILING_COMMAND_END>();
		timeTotal += timeEnd - timeStart;
	}
	std::cout << "Task C executed. Kernel enqueued." << std::endl;
	
	// read from output buffer
	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = imgWidth;
	region[1] = imgHeight;
	region[2] = 1;

	queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImage);

	write_BMP_RGBA_to_RGB("Task3c.bmp", outputImage, imgWidth, imgHeight);

	printf("Average run time = %lu\n", timeTotal / NUM_ITERATIONS);
	std::cout << "--------------------" << std::endl;
	std::cout << "Done." << std::endl;
}
