#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <format>
#include <algorithm>
#include <map>
#include <deque>
#include <cassert>

namespace D05 {
	using DstSrcRange = std::tuple<__int64, __int64, __int64>;
	using RangeMap = std::vector<DstSrcRange>;

	void part1(const std::vector<__int64>& seeds, const std::vector<RangeMap>& maps)
	{
		auto findOutput = [](const RangeMap& m, __int64 input) {
			for (const auto& [dstStart, srcStart, len] : m)
			{
				__int64 offset = input - srcStart;
				if (offset < 0) { continue; }
				if (offset < len) {
					return dstStart + offset;
				}
			}
			return input;
		};

		std::vector<__int64> inputs = seeds;
		std::vector<__int64> outputs;
		for (int i = 0; i < maps.size(); ++i)
		{
			for (__int64& input : inputs)
			{
				outputs.emplace_back(findOutput(maps[i], input));
			}
			std::swap(inputs, outputs);
			outputs.clear();
		}

		__int64 lowestLocation = *std::min_element(inputs.begin(), inputs.end());
		std::cout << lowestLocation << "\n";
		assert(lowestLocation == 3374647 || lowestLocation == 35);
	}

	void part2(const std::vector<__int64>& seeds, const std::vector<RangeMap>& maps)
	{
		struct Range
		{
			__int64 from{ std::numeric_limits<__int64>::max() / 2 };
			__int64 to{ std::numeric_limits<__int64>::min() / 2 };
			__int64 len() const { return (to - from) + 1; };
		};

		auto findOutput = [](const RangeMap& m, const Range& input, std::deque<Range>& redo) {
			for (const auto& [dstStart, srcStart, len] : m)
			{
				__int64 srcLeft = srcStart;
				__int64 srcRight = srcStart + len - 1;
				//outer
				if (input.to < srcLeft) { continue; }
				if (input.from > srcRight) { continue; }

				//full-inner
				if (input.from >= srcLeft && input.to <= srcRight)
				{
					__int64 offsetLeft = input.from - srcLeft;
					__int64 dst = dstStart + offsetLeft;
					__int64 dstEnd = dst + input.len() - 1; //! off by one?
					return Range{ dst,dstEnd };
				}

				//partial inner
				Range inner{ input.from,input.to };
				if (input.from < srcLeft)
				{
					Range left{ .from = input.from, .to = srcLeft - 1 };
					inner.from = srcLeft;
					redo.push_back(left);
				}
				if (input.to > srcRight)
				{
					inner.to = srcRight;
					Range right{ .from = srcRight + 1, .to = input.to };
					redo.push_back(right);
				}
				if (inner.len() > 0)
				{
					redo.push_back(inner);
				}
				return Range{};
			}
			//nohit
			return input;
		};

		std::vector<Range> inputs;
		for (size_t i = 1; i < seeds.size(); i += 2) {
			inputs.emplace_back(seeds[i - 1], seeds[i - 1] + seeds[i] - 1);
		}

		for (int i = 0; i < maps.size(); ++i)
		{
			std::vector<Range> outputs;
			for (const Range& input : inputs)
			{
				std::deque<Range> todo{ input };
				while (todo.size() > 0)
				{
					Range active = todo.front();
					todo.pop_front();

					Range result = findOutput(maps[i], active, todo);
					if (result.len() > 0) {
						outputs.emplace_back(result);
					}
				}
			}
			std::swap(inputs, outputs);
		}

		Range lowestLocation = *std::min_element(inputs.begin(), inputs.end(),
			[](const Range& l, const Range& r) { return l.from < r.from; });
		std::cout << lowestLocation.from << "\n";
		assert(lowestLocation.from == 6082852 || lowestLocation.from == 46);
	}
}

void aoc2023_05()
{
	using namespace D05;
	fs::path input(DataDir() / "2023_05.txt");
	TxtFile txt{ input };

	std::vector<__int64> seeds;
	std::vector<RangeMap> maps;
	int readState = 0;
	int mapIndex = -1;
	__int64 count{};
	for (const auto& line : txt)
	{
		if (line == "-----") { break; }
		if (readState == 0)
		{
			if (line.starts_with("seeds:")) {
				seeds = intSplit<' ', __int64, true>(line.substr(6));
				readState = 1;
				++count;
			}
		}
		else if (readState == 1)
		{
			if (line.find("map:") != std::string::npos)
			{
				maps.emplace_back(RangeMap{});
				++count;
				++mapIndex;
				readState = 2;
			}
		}
		else if (readState == 2)
		{
			if (line.length() == 0)
			{
				readState = 1; 
			}
			else {
				RangeMap& currentMap = maps[mapIndex];
				std::vector<__int64> tmp = intSplit<' ', __int64, true>(line);
				if (tmp.size() == 3)
				{
					currentMap.emplace_back(tmp[0], tmp[1], tmp[2]);
					++count;
				}
			}
		}
	}
	std::cout << "count: " << count << "\n";

	
	part1(seeds,maps);
	part2(seeds,maps);
}

