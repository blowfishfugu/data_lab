#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"

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
#include <array>

bool isNice(const std::string_view& line)
{
	//3 vowels
	if( std::count_if(line.cbegin(), line.cend(), 
		[](const char c) {
		constexpr std::string_view v = "aeiou";
		return v.find(c) != std::string_view::npos;}
	) < 3){
		return false; 
	}

	//at least one pair
	if (std::adjacent_find(line.cbegin(), line.cend()) == line.cend())
	{
		return false;
	}

	//and none of these
	std::array<std::string, 4> nonos{"ab","cd","pq","xy"};
	for (const std::string& nono : nonos)
	{
		if (line.find(nono) != std::string::npos)
		{
			return false;
		}
	}
	return true;
}

bool isNice2(const std::string_view& line)
{
	//at least one non-overlapping pair (xxxx) but not (xxxy)
	std::map<std::string_view, std::vector<size_t> > pairIndices;
	for (size_t i = 1; i < line.length(); ++i)
	{
		size_t start_pos = i - 1;
		std::string_view p{ line.substr(start_pos,2) };
		pairIndices[p].emplace_back(start_pos);
	}
	bool validPair = false;
	for (const auto& [aa, vec] : pairIndices)
	{
		if (vec.size() < 2) { continue; }
		if (vec.size() >= 3) { validPair = true; break; }
		if (vec.size() == 2)
		{
			if ((vec[0] + 1) != vec[1])
			{
				validPair = true; break;
			}
		}
	}
	if (!validPair) { return false; }

	//at least a pair with one in the middle (xyx)
	bool hasOneBetween = false;
	for (size_t i = 2; i < line.length(); ++i)
	{
		char c1 = line[i - 2];
		char c2 = line[i - 1];
		char c3 = line[i - 0];
		if (c1 != c2 && c1 == c3)
		{
			hasOneBetween = true;
		}
	}
	if (!hasOneBetween) { return false; }

	return true;
}


void aoc2015_05()
{
	fs::path input(DataDir() / "2015_05.txt");
	TxtFile txt{ input };

	__int64 nice = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { continue; }
		if (isNice(line))
		{
			++nice;
			std::cout << line << "\n";
		}
	}
	std::cout << "nice: " << nice << "\n";
	assert(nice == 236);

	__int64 nice2 = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { continue; }
		if (isNice2(line))
		{
			++nice2;
			std::cout << line << "\n";
		}
	}
	std::cout << "nice2: " << nice2 << "\n";
	assert(nice2 == 51);
}