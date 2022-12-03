#include "DataDir.h"
#include <iostream>
#include <array>
#include <string>
#include <algorithm>
#include <functional>
#include <numeric>
void aoc2022_03()
{
	fs::path input(DataDir() / "2022_03.txt");
	struct Item
	{
		char input='\0';
		__int64 left = 0;
		__int64 right = 0;
		__int64 exists = 0;
	};
	// [65-90], [97-122]
	using Hist = std::array<Item, 128>;
	std::vector<Hist> bags;

	std::ifstream inFile(input);
	__int64 score = 0;

	auto getScore=[](char c)->__int64 {
		if (c >= 'a') //[1-26]
		{
			__int64 prio = ((__int64)c - 'a') + 1;
			return prio;
		}
		//[27-52]
		__int64 prio = ((__int64)c - 'A') + 27;
		return prio;
	};

	for (std::string line; std::getline(inFile, line);)
	{
		size_t len = line.length();
		if (len == 0 ) { break; }
		bags.emplace_back(Hist{});
		Hist& hist = bags[bags.size() - 1];
		size_t half = len / 2;
		for (size_t current = 0; current < half; ++current)
		{
			char c = line[current];
			Item& item = hist[c];
			item.input = c;
			item.left++;
			item.exists=1;
		}
		for (size_t current = half; current < len; ++current)
		{
			char c = line[current];
			Item& item = hist[c];
			item.input = c;
			item.right++;
			item.exists = 1;
			if (item.left > 0 
				&& item.right == 1 //nur erste dublette zählen
				)
			{
				score += getScore(item.input);
			//early break?
			}
		}

		
	}
	__int64 badgeScore = 0;
	__int64 groupCount = 0;
	for (size_t i = 0; i < bags.size(); i += 3)
	{
		Hist& b1 = bags[i];
		Hist& b2 = bags[i + 1];
		Hist& b3 = bags[i + 2];
		groupCount++;
		for (size_t pos = 'A'; pos <='z'; ++pos)
		{
			__int64 isThere = b1[pos].exists + b2[pos].exists + b3[pos].exists;
			if (isThere == 3)
			{
				badgeScore += getScore(b1[pos].input);
				break;
			}
		}
	}

	std::cout << score << "\n";
	std::cout << badgeScore << "\n";
	
}