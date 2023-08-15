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
#include "MD5.h"

namespace {
	auto md5Func = [](const std::string& prefix, __int64 startIndex, const __int64 poolSize, __int64& result, std::atomic_bool& stop)
	{
		while (!stop)
		{
			MD5 calc;
			std::string str{ std::format("{}{}", prefix, startIndex) };
			std::string hash = calc.FromString(str.data(), str.length());
			if (hash.find("000000") == 0)
			{
				result = startIndex;
				stop = true;
				break;
			}
			startIndex += poolSize;
		}
	};
}


void aoc2015_04()
{
	fs::path input(DataDir() / "2015_04.txt");
	
	std::string startPrefix;
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0)
		{
			break;
		}
		startPrefix = line;
	}
	inFile.close();

	{ //check hashing
		std::string str{ "abcdef609043" };
		MD5 calc;
		std::string hash = calc.FromString(str.data(), str.length());
		assert(hash.find("00000") == 0);
	}

	const std::string prefix{ startPrefix };
#ifdef NDEBUG
	constexpr size_t threadCount = 63LL;
#else
	constexpr size_t threadCount = 1LL;
#endif
	std::vector<__int64> results;
	results.resize(threadCount);
	__int64 start{}; //<- optimierungsmöglichkeit, startIndex von Vorlauf, der nur 00000 gesucht hat
	__int64 offset = results.size();
	std::atomic_bool stop = false;
	
	std::vector<std::thread> threads;
	for (size_t threadIndex = 0LL; threadIndex < threadCount; ++threadIndex)
	{
		__int64 startIndex = start + threadIndex;
		__int64& result = results[threadIndex];
		threads.emplace_back(
			[prefix, startIndex, offset, &result , &stop](){
				md5Func(prefix, startIndex, offset, result, stop);
				}
			);
	}

	for (std::thread& t : threads) { t.join(); }
	
	//nullen rausfiltern, falls zwei gleichzeitig treffen, zusätzlich sortieren
	auto firstZero=std::partition(results.begin(), results.end(), [](const __int64& l) { return l > 0; } );
	std::sort(results.begin(), firstZero, [](const __int64& l, const __int64& r) {return l < r; });
	//std::cout << std::format("hit 00000 on {}\n", results[0]);
	//assert(results[0] == 254575);
	std::cout << std::format("hit 000000 on {}\n", results[0]);
	assert(results[0] == 1038736);
}