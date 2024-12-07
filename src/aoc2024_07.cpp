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
		I shift = 1000;
		if (r < 10) { shift = 10; }
		else if (r < 100) { shift = 100; }
		else if (r < 1000) { shift = 1000; }
		else if (r < 10000) [[unlikely]] { shift = 10000; }
		else if (r < 100000) [[unlikely]] { shift = 100000; }
		return l * shift + r;
		//return toInt<I>(std::format("{}{}", l, r));
	};

	template<bool withConcat>
	void solve (Equation& e) {
		I& validCount = std::get<2>(e);
		if (validCount > 0) { return; }

		const I& target = std::get<0>(e);
		const std::vector<I>& vals = std::get<1>(e);

		std::deque<std::tuple<I, size_t>> toProcess;
		toProcess.push_back({ vals[0], 1LL });
		while (toProcess.size() > 0){
			auto [left, nextIndex] = toProcess.front();
			toProcess.pop_front();

			I right = vals[nextIndex];
			++nextIndex;

			I mulled = left * right;
			I summed = left + right;
			I concated{};
			if constexpr (withConcat){
				concated = concat(left, right);
			}

			if (nextIndex >= vals.size()){
				if (mulled == target){
					validCount=1;
				}
				if (summed == target){
					validCount=1;
				}
				if constexpr (withConcat){
					if (concated == target){
						validCount=1;
					}
				}
				continue;
			}

			if (mulled <= target){
				toProcess.push_back({ mulled,nextIndex });
			}
			if (summed <= target){
				toProcess.push_back({ summed,nextIndex });
			}
			if constexpr (withConcat){
				if (concated <= target){
					toProcess.push_back({ concated,nextIndex });
				}
			}
		}
	}

}

void aoc2024_07()
{
	using std::execution::par;
	fs::path input(DataDir() / "2024_07.txt");
	TxtFile txt{ input };
	std::vector<Equation> calibrations;
	for (const auto& line : txt){
		if (line.length() == 0) { break; }
		SpaceIterator it{ line }; SpaceIterator end{};
		Equation e{};
		int i = 0;
		while (it != end){
			I val = toInt<I>(*it);
			if (i == 0) { std::get<0>(e) = val; }
			else { std::get<1>(e).emplace_back(val); }
			++it;
			++i;
		}
		calibrations.emplace_back(e);
	}
	std::println(std::cout, "eqCount: {}", calibrations.size());
	
	std::for_each(par, calibrations.begin(), calibrations.end(), 
		[](Equation& e){ solve<false>(e); //part1, mul and plus
	});

	auto countValid = [](I last, const Equation& e) {return last + std::get<0>(e) * std::get<2>(e); };
	I sum = std::accumulate(calibrations.cbegin(), calibrations.cend(), 0LL, countValid);
	std::println(std::cout, "Sum of valid test values: {}", sum);

	std::for_each(par, calibrations.begin(), calibrations.end(),
		[](Equation& e){ solve<true>(e); //remaining, part2, mul,plus and concat
	});

	I sum2 = std::accumulate(calibrations.cbegin(), calibrations.cend(), 0LL, countValid);
	std::println(std::cout, "Sum of extended test values: {}", sum2);
}

