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


//forward declarations
bool input_check(cl_float &threshold);
void output_4a(cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
	unsigned char *inputImage, unsigned char *outputImageD, int imgWidth, int imgHeight);
void output_4b_4c(cl::CommandQueue &queue, cl::Program &program, cl::Context &context, int pass_number,
	unsigned char *inputImageC, unsigned char *outputImageB, int imgWidth, int imgHeight, const char* outputFile);
void output_4d(cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
	unsigned char *inputImage, unsigned char *inputImageC, unsigned char *outputImageD, int imgWidth, int imgHeight);



int main(void)
{
	cl::Platform platform;			// device's platform
	cl::Device device;				// device used
	cl::Context context;			// context for the device
	cl::Program program;			// OpenCL program object
	cl::Kernel kernel;				// a single kernel object
	cl::CommandQueue queue;			// commandqueue for a context and device

	// declare data and memory objects
	unsigned char* inputImage;
	unsigned char *outputImageA, *outputImageB, *outputImageC,  *outputImageD;
	int imgWidth, imgHeight, imageSize;

	
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
		if (!build_program(&program, &context, "bloom.cl"))
		{
			// if OpenCL program build error
			quit_program("OpenCL program build error.");
		}

		// create command queue
		queue = cl::CommandQueue(context, device);

		// ============== Task 4a ================ //
		// read input image
		inputImage = read_BMP_RGB_to_RGBA("peppers.bmp", &imgWidth, &imgHeight);

		// allocate memory for output image
		imageSize = imgWidth * imgHeight * 4;
		outputImageA = new unsigned char[imageSize];
		outputImageB = new unsigned char[imageSize];
		outputImageC = new unsigned char[imageSize];
		outputImageD = new unsigned char[imageSize];

		// remove pixels below user-defined threshold
		output_4a(queue, program, context, inputImage, outputImageA, imgWidth, imgHeight);

		// two pass filter
		// first pass
		output_4b_4c(queue, program, context, 1, outputImageA, outputImageB, imgWidth, imgHeight, "Task4b.bmp");
		//second pass
		output_4b_4c(queue, program, context, 2, outputImageB, outputImageC, imgWidth, imgHeight, "Task4c.bmp");

		// clamp original image and image C
		output_4d(queue, program, context, inputImage, outputImageC, outputImageD, imgWidth, imgHeight);

		// deallocate memory
		free(inputImage);
		free(outputImageA);
		free(outputImageB);
		free(outputImageC);
		free(outputImageD);
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

// promt user for input threshold and validate it
bool input_check(cl_float &threshold) {

	std::cout << "Please enter a number from 0.0 to 1.0 (inclusive): ";

	std::string inputString;
	float input;	// option that was selected

	std::getline(std::cin, inputString);
	std::istringstream stringStream(inputString);

	// check whether valid option selected
		// check if input was an integer
	if (stringStream >> input)
	{
		char c;

		// check if there was anything after the integer
		if (!(stringStream >> c))
		{
			// check if valid option range
			if (0 <= input && input <= 1)
			{
				threshold = input;

				return true;
			}
		}
	}
	// if invalid option selected
	std::cout << "\n--------------------" << std::endl;
	std::cout << "Invalid input." << std::endl;

	return false;
}

// filter out pixels below threshold
void output_4a(cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
	unsigned char *inputImage, unsigned char *outputImageA, int imgWidth, int imgHeight) {

	// get and validate user input for the threshold
	cl_float threshold = 0.0;
	if (!input_check(threshold)) {
		// if input is invalid
		quit_program("Input is invalid.");
	}

	// image format
	cl::ImageFormat imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);

	// create a kernel
	cl::Kernel kernel = cl::Kernel(program, "glowing_pixels");

	// create image objects
	cl::Image2D inputImgBuffer = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)inputImage);
	cl::Image2D outputImgBuffer = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImageA);


	// set kernel arguments
	kernel.setArg(0, inputImgBuffer);
	kernel.setArg(1, threshold);
	kernel.setArg(2, outputImgBuffer);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(imgWidth, imgHeight);
	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Kernel enqueued. Dull pixels filtered out" << std::endl;
	std::cout << "--------------------" << std::endl;

	// enqueue command to read image from device to host memory
	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = imgWidth;
	region[1] = imgHeight;
	region[2] = 1;

	queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImageA);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task4a.bmp", outputImageA, imgWidth, imgHeight);

}

// two pass filter
void output_4b_4c(cl::CommandQueue &queue, cl::Program &program, cl::Context &context, int pass_number,
	unsigned char *inputImageA, unsigned char *outputImageB, int imgWidth, int imgHeight, const char* outputFile) {
	// image format
	cl::ImageFormat imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
	cl::Kernel kernel = cl::Kernel(program, "filter_two_passes");

	// create image objects
	cl::Image2D inputImgBuffer = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*) inputImageA);
	cl::Image2D outputImgBuffer = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*) outputImageB);

	// set kernel arguments
	kernel.setArg(0, inputImgBuffer);
	kernel.setArg(1, pass_number);
	kernel.setArg(2, outputImgBuffer);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(imgWidth, imgHeight);
	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Kernel enqueued. Blur pass number " << pass_number << " performed." << std::endl;
	std::cout << "--------------------" << std::endl;

	// enqueue command to read image from device to host memory
	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = imgWidth;
	region[1] = imgHeight;
	region[2] = 1;
	queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImageB);

	// output results to image file
	write_BMP_RGBA_to_RGB(outputFile, outputImageB, imgWidth, imgHeight);
}

// clamp images
void output_4d(cl::CommandQueue &queue, cl::Program &program, cl::Context &context,
	unsigned char *inputImage, unsigned char *inputImageC, unsigned char *outputImageD, int imgWidth, int imgHeight) {
	// image format
	cl::ImageFormat imgFormat = cl::ImageFormat(CL_RGBA, CL_UNORM_INT8);
	// create a kernel
	cl::Kernel kernel = cl::Kernel(program, "clamp_images");

	// create image objects
	cl::Image2D inputImgBuffer1 = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)inputImage);
	cl::Image2D inputImgBuffer2 = cl::Image2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)inputImageC);
	cl::Image2D outputImgBuffer  = cl::Image2D(context, CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR, imgFormat, imgWidth, imgHeight, 0, (void*)outputImageD);

	// set kernel arguments
	kernel.setArg(0, inputImgBuffer1);
	kernel.setArg(1, inputImgBuffer2);
	kernel.setArg(2, outputImgBuffer);

	// enqueue kernel
	cl::NDRange offset(0, 0);
	cl::NDRange globalSize(imgWidth, imgHeight);
	queue.enqueueNDRangeKernel(kernel, offset, globalSize);

	std::cout << "Kernel enqueued. Images clamped." << std::endl;
	std::cout << "--------------------" << std::endl;

	// enqueue command to read image from device to host memory
	cl::size_t<3> origin, region;
	origin[0] = origin[1] = origin[2] = 0;
	region[0] = imgWidth;
	region[1] = imgHeight;
	region[2] = 1;
	queue.enqueueReadImage(outputImgBuffer, CL_TRUE, origin, region, 0, 0, outputImageD);

	// output results to image file
	write_BMP_RGBA_to_RGB("Task4d.bmp", outputImageD, imgWidth, imgHeight);

}