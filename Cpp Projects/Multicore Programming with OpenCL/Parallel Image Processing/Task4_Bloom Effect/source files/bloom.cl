__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | 
      CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST; 

__constant float GaussianFilter_two_passes[7] = {0.00598, 0.060626, 0.241843, 0.383103, 0.241843, 0.060626, 0.00598};

// Task 4a
__kernel void glowing_pixels(read_only image2d_t src_image,
					float threshold,
					write_only image2d_t dst_image) {

   /* Get work-item’s row and column position */
   int2 coord;
   coord.x = get_global_id(0);
   coord.y = get_global_id(1);
   
   float4 pixel = read_imagef(src_image, sampler, coord);

   // calc average of RGB
   float luminance = 0.299 * pixel.x + 0.578 * pixel.y + 0.114 * pixel.z;
   
   if(luminance < threshold){
		pixel.xyz = 0;
   }
   
	write_imagef(dst_image, coord, pixel);
}

// Task 4b and 4c
__kernel void filter_two_passes(read_only image2d_t src_image,
					int pass_number,
					write_only image2d_t dst_image) {

   /* Get work-item’s row and column position */
	int column = get_global_id(0);
	int row = get_global_id(1);

	float4 sum = (float4)(0.0);

	int filter_index = 0;

	int2 coord = (int2)(column, row);
	float4 pixel;

	// first pass
	if (pass_number == 1) {
		for (int i = -3; i <= 3; i++)
		{
			coord.x = column + i;
			pixel = read_imagef(src_image, sampler, coord);
			sum.xyz += pixel.xyz * GaussianFilter_two_passes[filter_index++];
		}
	} else { //second pass
		for (int i = -3; i <= 3; i++){
			coord.y = row + i;
			pixel = read_imagef(src_image, sampler, coord);
			sum.xyz += pixel.xyz * GaussianFilter_two_passes[filter_index++];
		}
	}

	coord = (int2)(column, row);
	write_imagef(dst_image, coord, sum);
}

// Task 4d
__kernel void clamp_images(read_only image2d_t src_image1,
					read_only image2d_t src_image2,
					write_only image2d_t dst_image) {

   /* Get work-item’s row and column position */
   int2 coord;
   coord.x = get_global_id(0);
   coord.y = get_global_id(1);
   
   float4 pixel1 = read_imagef(src_image1, sampler, coord);
   float4 pixel2 = read_imagef(src_image2, sampler, coord);

   // add pixels
   float4 sum = pixel1 + pixel2;
   float4 pixel_output = clamp(sum, (float4) (0.0), (float4) (1.0));
   
	write_imagef(dst_image, coord, pixel_output);
}