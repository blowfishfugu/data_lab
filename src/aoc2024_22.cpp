#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <print>
#include <numeric>
#include <algorithm>
#include <execution>
#include <atomic>
#include <map>
#include <unordered_map>

using Sequence = std::tuple<int, int, int, int>;
using SeqMap = std::unordered_map<Sequence, int>;

template <>
class std::hash<Sequence>
{
public:
	std::uint64_t operator()(const Sequence& seq) const
	{
		std::uint64_t h = (
			std::get<0>(seq) << sizeof(int)   *3 
			^ std::get<1>(seq) << sizeof(int) *2 
			^ std::get<2>(seq) << sizeof(int)
			^ std::get<3>(seq)
			);
		return h;
	}
};

namespace D22 {
	using UI = std::uint64_t;
	using INT = std::int32_t;
	using VI = std::vector<UI>;
	constexpr UI secret_modulo = 16777216ULL;

	constexpr auto mix = [](UI secret, UI tmp) {
		return secret^tmp;
		};

	constexpr auto prune = [](UI secret) {
		return secret%secret_modulo;
		};

	constexpr auto step1 = [](UI secret) {
		UI mul = secret * 64;
		secret = mix(secret, mul);
		secret = prune(secret);
		return secret;
		};

	constexpr auto step2 = [](UI secret) {
		UI div = secret / 32; 
		//round down to nearest integer? 32Bit truncate or just do implicit int-result
		secret = mix(secret, div);
		secret = prune(secret);
		return secret;
		};

	constexpr auto step3 = [](UI secret) {
		UI mul = secret * 2048;
		secret = mix(secret, mul);
		secret = prune(secret);
		return secret;
		};

	constexpr auto buildNext = [](UI secret) {
		secret = step1(secret);
		secret = step2(secret);
		secret = step3(secret);
		return secret;
	};

	constexpr auto toPrice = [](const UI secret)->int {
		UI overTen = static_cast<UI>(std::floor((double)secret / 10.0)*10);
		return static_cast<int>(secret - overTen);
		};
	

	

	constexpr auto loop = [](UI& secret, const UI rounds)->SeqMap {
		using Price = std::tuple<int, int>;
		using Prices = std::vector<Price>;
		Prices p{}; p.reserve(rounds+1);
		p.emplace_back(Price{ toPrice(secret),0 });
		for (UI i = 0; i < rounds; ++i) {
			secret = buildNext(secret);
			//std::println(std::cout, "{:>4} : {}", i, secret );
			p.emplace_back(Price{toPrice(secret),0});
		}

		for (int i = 1; i < p.size(); ++i) {
			const Price& last = p[i - 1];
			auto& [current, delta] = p[i];
			delta = current - std::get<0>(last);
		}

		SeqMap seqs{};
		for (int i = 4; i < p.size(); ++i) {
			Sequence s{ 
				std::get<1>(p[i-3]),
				std::get<1>(p[i-2]),
				std::get<1>(p[i-1]),
				std::get<1>(p[i-0]),
			};
			if (!seqs.contains(s)) {
				seqs.emplace(s, std::get<0>(p[i - 0]));
			}
		}

		return seqs;
	};

}

void aoc2024_22()
{
	using namespace D22;
	fs::path input(DataDir() / "2024_22.txt");
	TxtFile txt{ input };

	///*UI tstXor = mix(42, 15);
	//assert(tstXor == 37);
	//UI tstPrune = prune(100000000);
	//assert(tstPrune == 16113920);*/

	__int64 count = 0LL;
	VI numbers{};
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		numbers.emplace_back(toInt<UI>(line));
		++count;
	}
	
	std::vector<SeqMap> monkeys;
	monkeys.resize(numbers.size());
	std::atomic<int> index{0};
	std::for_each( std::execution::par,
		numbers.begin(), numbers.end(), [&index,&monkeys](UI& secret) {
		int i = index.fetch_add(1, std::memory_order_relaxed);
		monkeys[i]= loop(secret, 2000LL);
	});

	UI sum=std::accumulate(numbers.begin(), numbers.end(), 0ULL);
	std::println(std::cout, "sum of secrets: {}", sum);

	int maxBananas{};
	SeqMap combined{};
	for (int i = 0; i < monkeys.size(); ++i) {
		const SeqMap& l = monkeys[i];
		for ( const auto& [seq, prize] : l) {
			combined[seq] += prize;
			maxBananas = std::max(maxBananas, combined[seq]);
		}
	}
	std::println(std::cout, "maxBananas: {}", maxBananas);
}

