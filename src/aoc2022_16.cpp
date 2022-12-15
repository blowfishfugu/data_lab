#include "DataDir.h"
#include <iostream>

void aoc2022_16()
{
	fs::path input(DataDir() / "2022_16.txt");
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0) { break; }
	}

	__int64 score = 0;
	std::cout << score << "\n";
}