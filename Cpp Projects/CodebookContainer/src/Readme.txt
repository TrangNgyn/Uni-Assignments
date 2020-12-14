libgenVal is a library file provided by the lecturer and should be used in this assignment.

There are 2 containers:
	1. Codebook sis for storing ordered lists of symbols making up a codeword.
	2. Codeword contains a collection of codewords

There are 2 symbol classes:
	1. The first symbol class Mint allows us to store a single integer value in the range 0 to p−1, 
	where p is a positive integer, the modulus. In this class,the subtraction operator should be overloaded
	to produce the typical integer result modulo p.
	2. The second symbol class Melt allows us to store a single lowercase letter from the English alphabet,
	with the subtraction operator overloaded to give 1 if the symbols are different and 0 if they are the same.
	There are p = 26 distinct possible symbols.
  Both should also have a special "zero" symbol, for
  Mint this will be the integer 0, while for Melt it will be a letter ’a’.

The Codeword container:
	This container should be used to store elements of the same type. It should be a templated container
	class. The following methods should also be provided.
		1. The method Weight should determine the number of elements in the code that are not{equal to the
		"zero" symbol. For example, 0 2 0 3 3 3 0 has a weight of 4.
		2. The method Distance should take another codeword and determine the sum of the element by
		element difference according to the overloaded subtraction operation for the contained symbol class.
		For example, the distance between 0 1 2 3 0 1 and 0 0 2 0 0 2 for a Mint container with p = 7
		would be determined as
			0 1 2 3 0 1
		      - 0 0 2 0 0 2
		       --------------
			0+1+0+3+0+6 = 10

		3. The method Display should output the elements in the codeword, each separated by a space, with
		a final gap and the weight of the codeword displayed. For example,
			4 6 9 11 3 Weight: 5

The Codebook container
	This container should be used to store collections of codewords. It should be a templated container class.
	The following methods should be provided.
		1. The method minimumWeight should determine the minimum Weight value across all non-zero codewords.
		2. The method calcDistance should determine the distances between every pair of codewords in the
		code, and store these values.
		3. The method minimumDistance should determine the minimum Distance between two codewords in
		the code, as determined across all distinct pairs of codewords.
		4. The method Display should display all the codewords contained in the container, using the Display
		method for the codewords themselves, and display the minimum weight and minimum distance for
		this code. The table of distances between codewords should be displayed also.
	This container should contain a zero codeword, with the elements being all "zero", in accordance with
	the zero element for the contained symbol class.

Once the program is compiled into the executable CFC it must run as follows:
	./CFC 0 seed length size modulus
	./CFC 1 seed length size
	where the parameters have the following meanings:
		• First argument : (0 for Mint, 1 for Melt).
		• seed : A positive integer. Random seed for use in the value generator functions.
		• length : A positive integer. The number of symbols in each codeword.
		• size : A positive integer. The number of codewords in each codebook.
		• modulus : A positive integer, only relevant for the Mint code

The program should check all parameters entered before running. It uses c++11 standard library so make sure to specify the version when compiling.

To compile the program, run:
	CC -std=c++11 driver.cpp functions.cpp libgenVal.a -o CFC
