__kernel void encryptLookupTable(__global char4 *input,
								__global char *LUT,
								__global char *output) {
	
	int i = get_global_id(0);

	//convert scalar array to vector
	__private char4 inputVec = input[i];
	__private char4 uppercaseVec;
	__private char4 encryptedVec;
	
	//convert to uppercase for lower-case alphabetic characters
	if(any(inputVec >= (char4)('a') && inputVec <= (char4)('z'))){		
		// A: 65, a: 97 ;  a - A = 32
		char4 uppercase = (char4) (inputVec - (char4)(32));
		uppercaseVec = select(inputVec, uppercase,
						inputVec >= (char4)('a') && inputVec <= (char4)('z'));
	}else{
		uppercaseVec = inputVec;
	}

	//consider the index of a = 0, b = 1, ... p = 15 in look up table
	if(any(uppercaseVec >= (char4)('A') && uppercaseVec <= (char4)('Z'))){
		__private uint4 indexLUT = convert_uint4(uppercaseVec) - 65;		
		encryptedVec = select(uppercaseVec,
							(char4) (LUT[indexLUT.s0], LUT[indexLUT.s1], LUT[indexLUT.s2], LUT[indexLUT.s3]),
							uppercaseVec >= (char4)('A') && uppercaseVec <= (char4)('Z'));
	}else{
		encryptedVec = uppercaseVec;
	}

	//store in output scalar array
	vstore4(encryptedVec, i, output);
}
