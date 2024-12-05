#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <array>
#include <algorithm>
#include <numeric>
#include <print>

namespace {
	static auto contains = [](std::vector<int>& iVec, int iTest) {
		return (iVec.cend() != std::find_if(iVec.cbegin(), iVec.cend(),
			[&iTest](int i) { return i == iTest; })
			);
		};
}

void aoc2024_05()
{
	fs::path input(DataDir() / "2024_05.txt");
	TxtFile txt{ input };

	struct Page {
		int ID{};
		std::vector<int> isLessThan{};
	};

	static std::array<Page, 100> OrderingRules{};

	auto addRule = [](int iLesser, int iBigger) {
		Page& lesser = OrderingRules[iLesser];
		if (lesser.ID == 0) { lesser.ID = iLesser; }
		Page& bigger = OrderingRules[iBigger];
		if (bigger.ID == 0) { bigger.ID = iBigger; }
		lesser.isLessThan.emplace_back(iBigger);
	};

	//enhancement: operator<=> ? unmapped are equal
	auto isLesserOrEqual = [](int iPage, int iTest)->bool{
			Page& p = OrderingRules[iPage];
			Page& tst = OrderingRules[iTest];
			if (p.ID == 0) { return true; }
			if (tst.ID == 0) { return true; }

			if ( contains(p.isLessThan, tst.ID) )
			{
				return true;
			}
			return false;
		};

	using Update = std::vector<int>;
	std::vector<std::tuple<bool,Update>> testset{};
	int readState = 0;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { ++readState; continue; }
		if (readState > 1) { break; }
		if( readState==0){
			SplitIterator<'|'> it(line);
			int iLesser = toInt<int>(*it);
			++it;
			int iBigger = toInt<int>(*it);
			addRule(iLesser, iBigger);
		}
		else if( readState==1){
			Update update;
			SplitIterator<','> it(line); SplitIterator<','> end{};
			while (it != end) {
				update.emplace_back(toInt<int>(*it));
				++it;
			}
			testset.emplace_back( true,update );
		}
	}

	auto isRowOrdered = [&isLesserOrEqual](const Update& row)->bool {
		for (int i = 0; i < row.size(); ++i)
		{
			int page = row[i];
			for (int j = i + 1; j < row.size(); ++j) {
				if (!isLesserOrEqual(page, row[j]))
				{
					return false;
				}
			}
		}
		return true;
	};

	std::int64_t midpoints{};
	for ( auto& test : testset) {
		const Update& row = std::get<1>(test);
		if (row.size() == 0) { continue; }
		bool& valid = std::get<0>(test);
		valid = isRowOrdered(row);
		if( valid )
		{
			size_t mid=std::midpoint(0ULL,row.size());
			midpoints += row[mid];
		}
	}

	std::println("sumofMids: {}", midpoints);
	
	std::int64_t sumOfReordered{};
	for (auto& test : testset) {
		auto& [ordered, row] = test;
		if (ordered) { continue; }
		if (row.size() == 0) { continue; }
		std::sort(row.rbegin(), row.rend(), 
			//sorting "forwards" throws invalid comparator-error
			//isLesserOrEqual assumes unmapped IDs as equal -> 42<99 && 99<42 can't be true
			[&isLesserOrEqual](int l, int r) { return !isLesserOrEqual(l, r); 
			});
		if (isRowOrdered(row))
		{
			size_t mid = std::midpoint(0ULL, row.size());
			sumOfReordered += row[mid];
		}
	}

	std::println("sumofReordered: {}", sumOfReordered);
}

