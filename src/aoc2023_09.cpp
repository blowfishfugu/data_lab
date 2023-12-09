#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>

void aoc2023_09()
{
	fs::path input(DataDir() / "2023_09.txt");
	TxtFile txt{ input };

	__int64 count = 0LL;
	using seq = std::vector<__int64>;
	std::vector< seq > report;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		report.emplace_back(intSplit<' ', __int64, true>(line));
		++count;
	}
	std::cout << "count: " << count << "\n";
	
	auto shrink = [](const seq& in) -> seq{
		seq result{};
		if (in.size() == 0) { return result; }
		result.reserve(in.size());
		result.resize(in.size()-1);
		for (size_t i = 1; i < in.size(); ++i)
		{
			result[i-1]=(in[i] - in[i - 1]);
		}
		return result;
	};

	auto expand = [](const seq& prev, seq& target, __int64 firstOfPrev)
		->std::tuple<__int64, __int64>
	{
		size_t pos = prev.size() - 1;
		__int64 lastOfPrev = prev[pos];
		__int64 newRight = lastOfPrev + target[pos];
		target.push_back(newRight);

		__int64 newLeft = target[0] - firstOfPrev;
		return {newLeft, newRight};
	};

	__int64 sum{};
	__int64 sum2{};
	for (const seq& top : report)
	{
		if (top.size() == 0) { continue; }

		std::vector<seq> calced;
		calced.resize(top.size());
		calced[0]=top;
		for (size_t level = 0; level < top.size()-1; ++level)
		{
			calced[level+1]=shrink(calced[level]);
		}

		for (const seq& tst : calced)
		{
			for (__int64 x : tst) { std::cout << std::format("{:^7} ",x); }
			std::cout << "\n";
		}

		calced[calced.size() - 1].push_back(0LL);
		__int64 tmpLeft= calced[calced.size() - 1].front();
		for (size_t level = top.size()-1; level>=1; --level)
		{
			auto [newLeft,newRight]=expand(calced[level],calced[level-1], tmpLeft);
			std::cout << newLeft << " \\../ " << newRight << "\n";
			tmpLeft = newLeft;
		}
		

		__int64 expansion= calced.front().back();
		sum += expansion;
		sum2 += tmpLeft;
	}
	std::cout << sum << "\n";
	std::cout << sum2 << "\n";
}

