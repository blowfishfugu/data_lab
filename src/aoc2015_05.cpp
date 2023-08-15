#include "DataDir.h"
#include <iostream>
#include <format>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>
#include <string_view>
#include <algorithm>
#include <functional>
#include <numeric>
#include <execution>
#include <thread>

void aoc2015_05()
{
	fs::path input(DataDir() / "2015_05.txt");
	std::vector<std::string> lines;
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0)
		{
			break;
		}
		lines.emplace_back(line);
	}
	inFile.close();
}