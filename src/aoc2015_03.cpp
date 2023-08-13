#include "DataDir.h"
#include <iostream>
#include <format>
#include <vector>
#include <cassert>
#include <string_view>
#include <algorithm>
#include <functional>
#include <numeric>
#include <execution>

void aoc2015_03()
{
	fs::path input(DataDir() / "2015_03.txt");
	
	
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0)
		{
			break;
		}
	}
	inFile.close();
	
	
}