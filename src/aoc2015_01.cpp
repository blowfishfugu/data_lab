#include "DataDir.h"
#include <iostream>

void aoc2015_01()
{
	fs::path input(DataDir() / "2015_01.txt");
	
	std::ifstream inFile(input);
	__int64 curSum = 0;
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0)
		{
			break;
		}

		++curSum;
	}

	std::cout << curSum << " Lines \n";
	
}