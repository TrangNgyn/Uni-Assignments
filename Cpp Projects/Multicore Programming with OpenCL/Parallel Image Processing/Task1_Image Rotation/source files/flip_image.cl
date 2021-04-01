__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | 
      CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST; 

__kernel void flip_image(	read_only image2d_t src_image,
							write_only image2d_t dst_image1,
							write_only image2d_t dst_image2,
							write_only image2d_t dst_image3 ) {

   /* Get pixel coordinate */
   int imgWidth = get_image_width(src_image);
   int2 coord = (int2)(get_global_id(0), get_global_id(1));
   int2 new_coord;
   new_coord.x = imgWidth - coord.x - 1;
   new_coord.y = imgWidth - coord.y - 1;

   /* Read pixel value */
	float4 pixel = read_imagef(src_image, sampler, coord);

	/* flip horizontally */
   	/* Write new pixel value to output */
	write_imagef(dst_image1, (int2) (new_coord.x, coord.y), pixel);

	// flip vertically
	/* Write new pixel value to output */
	write_imagef(dst_image2, (int2) (coord.x, new_coord.y), pixel);

	// Flip third output image
	write_imagef(dst_image3, new_coord, pixel);   

}
