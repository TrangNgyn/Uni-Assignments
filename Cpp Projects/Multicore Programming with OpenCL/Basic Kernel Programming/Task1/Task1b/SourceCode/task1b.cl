__kernel void select_vec(__global int4 *input1,
						__global int *input2,
						__global int *output){

	//get index of current vec1 element in work-item
	int i = get_global_id(0);

	//declare vectors
	__private int8 v = (int8)(input1[i*2], input1[i*2+1]);
	__private int8 v1 = vload8(0, input2);
	__private int8 v2 = vload8(1, input2);
	int8 results;
	
	if(any(v > 17)){
		results = select(v2, v1, v > 17);
	}else{
		results = (int8) (v1.lo, v2.lo);
	}
	
	//store v, v1, v2 ad results in output array
	int offset = i * 4;
	vstore8(v, offset, output);
	vstore8(v1, offset + 1, output);
	vstore8(v2, offset + 2, output);
	vstore8(results, offset + 3, output);
}