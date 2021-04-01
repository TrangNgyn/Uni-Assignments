__kernel void initArr(__global int *arr,
					 int input) {

   int i = get_global_id(0);
   arr[i] = 3 + input * i;
}