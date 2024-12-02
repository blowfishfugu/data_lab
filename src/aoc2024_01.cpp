#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <algorithm>
#include <map>

void aoc2024_01()
{
	using I = std::int64_t;
	using VI = std::vector<I>;
	using Hist=std::map<I, I>;

	fs::path input(DataDir() / "2024_01.txt");
	TxtFile txt{ input };
	
	VI left{};
	VI right{};
	
	for (const auto& line : txt)
	{
		if (line.length() == 0) [[unlikely]] { break; }
		SpaceIterator it{ line };
		left.emplace_back( toInt<I>(*it) );
		++it; ++it; ++it;
		right.emplace_back( toInt<I>(*it) );
	}
	
	std::sort(left.begin(), left.end());
	std::sort(right.begin(), right.end());

	I sum{};
	Hist rMap; //part2, count of occurence
	for (size_t index = 0ULL; index < left.size();++index) {
		sum += std::abs( left[index] - right[index]);
		++rMap[right[index]];
	}
	
	I score{};
	for (I i : left) {
		score += (i * rMap[i]);
	}

	std::cout << std::format("sum of distances: {}\n", sum);
	std::cout << std::format("similarity score: {}\n", score);

}

