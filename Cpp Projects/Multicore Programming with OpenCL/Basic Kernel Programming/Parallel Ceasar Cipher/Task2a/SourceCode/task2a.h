#pragma once
#ifndef _TASK2A_H_
#define _TASK2A_H_
 
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


// function to quit program
void quit_program(const std::string str);

//obtain user's input for "n" and check for valid input
bool select_number(int &input);

// This function receives text and shift and 
// returns the encrypted text 
std::string encrypt(std::string text, int n);

#endif
