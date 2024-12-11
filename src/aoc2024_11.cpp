#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <list>
#include <numeric>
#include <cmath>
#include <format>
#include <print>
#include <execution>
#include <map>

namespace {
	using I = std::int64_t;
	using V = std::map<I,I>;//strange name? in part1 this was a vector
	struct Stone {
		I index{};
		V values{};
		I step{};
	};

	auto increaseZero = [](I i, I cnt, V& newvalues)->bool {
		if (i == 0) { newvalues[1] += cnt; return true; }
		return false;
	};

	auto evenDigits = [](I i, I cnt, V& newvalues)->bool {
		I count = 1; //todo: speedtest: floor(log10())+1 ?
		I tmp = i;
		while (tmp > 9) {
			tmp /= 10;
			++count;
		}

		if (!(count & 0x01))
		{
			I shift = static_cast<I>(std::pow(10, count / 2));
			I l = i / shift;
			I r = i - (l * shift);
			newvalues[l] += cnt;
			newvalues[r] += cnt;
			return true;
		}
		return false;
	};

	auto mutate = [](Stone& s) {
		V newvalues{};
		for (auto& [val, countOfVal] : s.values)
		{
			if (!increaseZero(val, countOfVal, newvalues)) {
				if (!evenDigits(val, countOfVal, newvalues)) {
					//else
					newvalues[val * 2024] += countOfVal;
				}
			}
		}
		++s.step;
		std::swap(s.values, newvalues);
	};

	auto printSum = [](std::vector<Stone>& stones, I step) {
		I sum{};
		for (Stone& s : stones)
		{
			for (auto& [_, cnt] : s.values) {
				sum += cnt;
			}
		}
		std::println(std::cout, "StoneCount at {}: {:>20}",step, sum);
	};

}

template<>
struct std::formatter<Stone> {
	constexpr auto parse(std::format_parse_context& context) {
		return context.begin();
	}
	auto format(const Stone& sVal, std::format_context& context) const {
		auto first= std::format_to(context.out(), "{} <{}>:", sVal.step, sVal.index);
		for (const auto& [val,count] : sVal.values) {
			first= std::format_to(first, " {} [{}]", val,count);
		}
		return first;
	}

};

void aoc2024_11()
{
	fs::path input(DataDir() / "2024_11.txt");
	TxtFile txt{ input };

	std::vector<Stone> stones;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		SpaceIterator it{ line }; SpaceIterator end{};
		size_t index{};
		while (it != end) {
			Stone s{};
			s.index = index++;
			s.values[toInt<I>(*it)]++;
			stones.emplace_back(s);
			++it;
		}
	}
	

	for (I step = 1; step <= 75; ++step)
	{
		std::for_each(std::execution::par,
			stones.begin(), stones.end(), [](Stone& s) {
			mutate(s);
			//std::println(std::cout, "{}", s);
		});
		//std::println(std::cout, "{:>10} : {:>10}", step, std::accumulate(stones.cbegin(), stones.cend(), 0LL, [](I l, const Stone& s) {return l + s.values.size(); }));
	
		if (step == 25) {
			printSum(stones, step);
		}
		if (step == 75) {
			printSum(stones, step);
		}
	}
}

