#include "DataDir.h"
#include <iostream>

void aoc2022_23()
{
	fs::path input(DataDir() / "2022_23.txt");
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0) { break; }
	}
//build net
//capture distances [..,..]
//walk paths (skip opened), lookup costs, calc income
	//??
	__int64 score = 0;
	std::cout << score << "\n";
}