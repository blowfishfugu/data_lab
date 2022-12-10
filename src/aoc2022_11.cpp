#include "DataDir.h"
#include <iostream>
#include <array>

void aoc2022_11()
{
	fs::path input(DataDir() / "2022_11.txt");
	std::ifstream inFile(input);
	__int64 score = 0;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0 ) { break; }
	}
	std::cout << score << "\n";

}