1. Make sure you have these files in the same directory:
	driver.cpp
	functions.h
	functions.cpp
	Artefacts.txt
	Chrononauts.txt
	Chronopets.txt
2. Go to the directory containing those files and compile the program:
	CC -std=c++11 driver.cpp functions.cpp -o AAT
3. To run the program, execute:
	./AAT number-of-artefacts number-of-jumps
	where:
		number-of-artefacts is an integer in range [1,5]
		number-of-jumps is an integer in range [1, 10]