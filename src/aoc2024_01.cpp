#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <numeric>
#include <map>

void aoc2024_01()
{
	fs::path input(DataDir() / "2024_01.txt");
	TxtFile txt{ input };

	using I = __int64;
	using VI = std::vector<I>;
	VI left{};
	VI right{};
	left.reserve(1000);
	right.reserve(1000);

	I count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		SpaceIterator it{ line }; SpaceIterator end{};
		left.emplace_back( toInt<I>(*it) );
		++it; ++it; ++it;
		right.emplace_back( toInt<I>(*it) );
		++count;
	}
	std::cout << std::format("count: {}\n", count);
	assert(count == 1000);

	std::sort(left.begin(), left.end());
	std::sort(right.begin(), right.end());
	assert(left.size() == right.size());

	I sum{};
	for (size_t index = 0ULL; index < left.size();++index) {
		sum+= std::abs( left[index] - right[index]);
	}
	std::cout << std::format("sum of distances: {}\n", sum);

	using Hist=std::map<I, I>;
	Hist rMap;
	for (I i : right) {
		rMap[i]++;
	}

	I score{};
	for (I i : left) {
		score += (i * rMap[i]);
	}
	std::cout << std::format("similarity score: {}\n", score);

}

