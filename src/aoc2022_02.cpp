#include "DataDir.h"
#include <iostream>

void aoc2022_02()
{
	fs::path input(DataDir() / "2022_02.txt");

	std::ifstream inFile(input);
	
	for (std::string line; std::getline(inFile, line);)
	{
	}
}