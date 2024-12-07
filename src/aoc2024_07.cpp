#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "SplitIterator.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <print>
#include <deque>
#include <execution>

namespace {
	using I = std::int64_t;
	using Equation = std::tuple<I, std::vector<I>,I>;
	
	static inline auto concat = [](I l, I r)->I {
		return toInt<I>(std::format("{}{}", l, r));
	};

	template<bool withConcat>
	void solve (Equation& e) {
		const I& target = std::get<0>(e);
		const std::vector<I>& vals = std::get<1>(e);
		I& validCount = std::get<2>(e);
		if (validCount > 0) { return; }

		std::deque<std::tuple<I, size_t>> toProcess;
		toProcess.push_back({ vals[0], 1LL });
		while (toProcess.size() > 0)
		{
			auto [left, nextIndex] = toProcess.front();
			toProcess.pop_front();
			if (left > target)
			{
				continue;
			}

			I right = vals[nextIndex];
			++nextIndex;

			I mulled = left * right;
			I summed = left + right;
			I concated{};
			if constexpr (withConcat) {
				concated = concat(left, right);
			}

			if (nextIndex >= vals.size())
			{
				if (mulled == target) {
					++validCount;
				}
				if (summed == target) {
					++validCount;
				}
				if constexpr (withConcat)
				{
					if (concated == target) {
						++validCount;
					}
				}
				continue;
			}

			toProcess.push_back({ mulled,nextIndex });
			toProcess.push_back({ summed,nextIndex });
			if constexpr (withConcat) {
				toProcess.push_back({ concated,nextIndex });
			}
		}
	}

}

void aoc2024_07()
{
	fs::path input(DataDir() / "2024_07.txt");
	TxtFile txt{ input };
	std::vector<Equation> calibrations;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		SpaceIterator it{ line }; SpaceIterator end{};
		Equation e{};
		int i = 0;
		while (it != end)
		{
			I val = toInt<I>(*it);
			if (i == 0) { std::get<0>(e) = val; }
			else { std::get<1>(e).emplace_back(val); }
			++it;
			++i;
		}
		calibrations.emplace_back(e);
	}
	std::println(std::cout, "eqCount: {}", calibrations.size());


	
	std::for_each(std::execution::par,
		calibrations.begin(), calibrations.end(), [](Equation& e)
		//for (Equation& e : calibrations)
		{
			solve<false>(e);
		});

	I sum{};
	for (const auto& [target, input, valid] : calibrations)
	{
		if (valid > 0)
		{
			sum += target;
		}
	}
	std::println(std::cout, "Sum of valid test values: {}", sum);

	std::for_each(std::execution::par,
		calibrations.begin(), calibrations.end(), [](Equation& e)
		//for (Equation& e : calibrations)
		{
			solve<true>(e);
		});

	sum = 0LL;
	for (const auto& [target, input, valid] : calibrations)
	{
		if (valid > 0)
		{
			sum += target;
		}
	}
	std::println(std::cout, "Sum of extended test values: {}", sum);
}

