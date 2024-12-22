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

	constexpr auto loop = [](UI& secret, const UI rounds) {
		for (UI i = 0; i < rounds; ++i) {
			secret = buildNext(secret);
			//std::println(std::cout, "{:>4} : {}", i, secret );
		}
	};

}

void aoc2024_22()
{
	using namespace D22;
	fs::path input(DataDir() / "2024_22.txt");
	TxtFile txt{ input };

	UI tstXor = mix(42, 15);
	assert(tstXor == 37);

	UI tstPrune = prune(100000000);
	assert(tstPrune == 16113920);

	__int64 count = 0LL;
	VI numbers{};
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		numbers.emplace_back(toInt<UI>(line));
		++count;
	}
	
	std::for_each( std::execution::par,
		numbers.begin(), numbers.end(), [](UI& secret) {
		loop(secret, 2000LL);
	});

	UI sum=std::accumulate(numbers.begin(), numbers.end(), 0ULL);
	std::println(std::cout, "sum of secrets: {}", sum);
}

