#include "DataDir.h"
#include <iostream>
#include <format>

#include <cassert>

void aoc2015_01()
{
	fs::path input(DataDir() / "2015_01.txt");
	
	std::ifstream inFile(input);
	__int64 floor = 0LL;
	__int64 current = 1LL;
	__int64 firstEntered = 0LL;
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0)
		{
			break;
		}

		for (const char c : line)
		{
			if (c == '(') { ++floor; }
			if (c == ')') { --floor; }
			if ( firstEntered==0LL && floor == -1LL )
			{ 
				firstEntered = current;
			}
			++current;
		}
	
	}
	std::cout << std::format("final floor: {}\n", floor);
	std::cout << std::format("entered floor -1 on: {}\n", firstEntered);
	assert(floor == 232);
	assert(firstEntered == 1783);
	
}