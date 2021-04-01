__kernel void encryptCaesar(__global char4 *input,
						int n,
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
	
	//encrypt upper-case alphabetic characters
	if(any(uppercaseVec >= (char4)('A') && uppercaseVec <= (char4)('Z'))){

		//position of characters in relation to the alphabet
		int4 ascii = ((convert_int4(uppercaseVec) + (int4)('A') + n) % (int4)(26) + 26) % 26 + 65;

		//encrypt the text
		char4 encrypted = convert_char4_sat(ascii);
		encryptedVec = select(uppercaseVec, encrypted, 
							uppercaseVec >= (char4)('A') && uppercaseVec <= (char4)('Z'));
	}else{
		encryptedVec = uppercaseVec;
	}

	//store in output scalar array
	vstore4(encryptedVec, i, output);
}