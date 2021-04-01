__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | 
      CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST; 

// 7x7 Gaussian blurring filter
__constant float GaussianFilter[49] = {0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036,
									   0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363,
									   0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446,
									   0.002291, 0.023226, 0.092651, 0.146768, 0.092651, 0.023226, 0.002291,
									   0.001446, 0.014662, 0.058488, 0.092651, 0.058488, 0.014662, 0.001446,
									   0.000363, 0.003676, 0.014662, 0.023226, 0.014662, 0.003676, 0.000363,
									   0.000036, 0.000363, 0.001446, 0.002291, 0.001446, 0.000363, 0.000036 };

__constant float GaussianFilter_two_passes[7] = {0.00598, 0.060626, 0.241843, 0.383103, 0.241843, 0.060626, 0.00598};


__kernel void filter(read_only image2d_t src_image,
					write_only image2d_t dst_image) {

   /* Get work-item’s row and column position */	
	int column = get_global_id(0);
	int row = get_global_id(1);

	float4 sum = (float4)(0.0);

	int filter_index = 0;

	int2 coord;
	float4 pixel;
	
	// Iterate through row
	for (int i = -3; i <= 3; i++)
	{
		coord.y = row + i;

		// Iterate through column
		for (int j = -3; j <= 3; j++)
		{
			coord.x = column + j;
			pixel = read_imagef(src_image, sampler, coord);
			sum.xyz += pixel.xyz * GaussianFilter[filter_index++];
		}
	}

	// write to output
	coord = (int2)(column, row);
	write_imagef(dst_image, coord, sum);
}

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

__kernel void filter_1D(read_only image2d_t src_image,
					write_only image2d_t dst_image) {

   /* Get work-item’s row and column position */
	int imgWidth = get_image_width(src_image);
	int global_id = get_global_id(0);
	int row = global_id / imgWidth;
	int column = global_id - row * imgWidth;

	float4 sum = (float4)0.0;

	int filter_index = 0;

	int2 coord;
	float4 pixel;

	for (int i = -3;i <= 3;i++)	{
		coord.y = row + i;

		for (int j = -3;j <= 3;j++)		{
			coord.x = column + j;
			pixel = read_imagef(src_image, sampler, coord);
			sum.xyz += pixel.xyz * GaussianFilter[filter_index++];
		}
	}

	coord = (int2)(column, row);
	write_imagef(dst_image, coord, sum);
}