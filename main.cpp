#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2015.h"

#include <string_view>
#include <string>
#include <charconv>
#include <format>
#include <array>
#include <map>
#include <functional>
using namespace std::literals;

auto toCents = [](const std::string_view& init) {
	std::ptrdiff_t toEnd = std::distance(init.cbegin(), init.cend());
	if (toEnd == 0) { return __int64{}; };

	__int64 in_major{};
	auto [dot, err] = std::from_chars(init.data(), init.data() + toEnd, in_major);
	if (dot && (*dot == '.' || *dot == ',')) {
		__int64 in_cents{};
		++dot;
		auto [centsEnd, err] = std::from_chars(dot, init.data() + toEnd, in_cents);
		if (err == std::errc{})
		{
			return in_major * 100 + in_cents;
		}
	}
	return in_major * 100;
};


class Coin {
public:
	explicit Coin(__int64 val) : cents{ val } {}
	const __int64 cents{};
	auto operator<=>(const Coin& other) const {	return cents <=> other.cents; }
	std::function<const std::string()> Name = [&] { return std::format("{:>6}", cents); };
	std::function<const std::string()> Plural = [] { return ""; };
};

template <__int64 val>
class Derivate : public Coin {
public:
	Derivate() : Coin{ val } {
		Name = [] {
			if constexpr (val < 500 || (val % 100) != 0) {
				return std::format("{:>6} Cent Münze", val);
			}
			else if constexpr (val == 900) {
				return std::format("{:>3} Euro Blüte", val / 100);
			}
			else {
				return std::format("{:>3} Euro Schein", val / 100);
			}
		};
		Plural = [] { 
			if constexpr (val < 500 || val == 900) {
				return "n";
			}
			else
			{
				return "e";
			}
		};
	}
};


class Money {
	__int64 cents{};
public:
	Money(const std::string_view& init) : cents{ toCents(init) } {}
	Money(__int64 c) : cents{ c } {}
	Money(Money&& in) noexcept { std::swap(cents, in.cents); }

	auto operator<=>(const Money& rhs) const = default;
	Money operator-(const Money& rhs) const { return cents - rhs.cents; }

	auto operator<=>(const Coin& other) const { return cents <=> other.cents; };
	Money& operator-=(const Coin& other) { cents -= other.cents; return *this; }
	Money& operator+=(const Coin& other) { cents += other.cents; return *this; }
};

const std::array<Coin, 13> knownCoins = {
	Derivate<1>{},
	Derivate<2>{},
	Derivate<5>{},
	Derivate<10>{},
	Derivate<20>{},
	Derivate<50>{},
	Derivate<100>{},
	Derivate<200>{},
	Derivate<500>{},
	Derivate<900>{},
	Derivate<1000>{},
	Derivate<2000>{},
	Derivate<5000>{},
};

auto convertToCoins = [](Money&& toSplit) {
	std::map<Coin, __int64> coins;
	auto ret = knownCoins.rbegin();
	while (ret != knownCoins.rend()) {
		if (toSplit >= *ret) {
			toSplit -= *ret;
			coins[*ret]++;
			continue;
		}
		++ret;
	}
	return coins;
};

auto calcResult(const std::string_view label, Money&& toSplit) {
	std::map<Coin, __int64> wanted = convertToCoins(std::forward<Money>(toSplit));
	std::cout << std::format("{}\n", label);
	for (const auto& [coin, count] : wanted) {
		std::cout << std::format("{}x {}{}\n", count, coin.Name(), count>1?coin.Plural():"");
	}
};

int main(int argc, char** argv)
{
	std::setlocale(LC_CTYPE, "de_DE");
	std::locale::global(std::locale(""));
	std::cout.imbue(std::locale());

	StopWatch clk;
	if (argc < 3) {
		std::cout << "expected <Euro.cents> <Wanted.cents>\n";
		calcResult("Demo: ", Money{ "50"sv } - "30.54"sv);
		return 0;
	}

	std::cout << "got = " << std::quoted(argv[1]) << "\n";
	std::cout << "want= " << std::quoted(argv[2]) << "\n";

	Money got(argv[1]);
	Money want(argv[2]);
	if (got < want) {
		calcResult("Please insert: ", want-got );
	}
	else {
		calcResult("Returning: ", got-want );
	}


	//aoc2015_01();clk.printDelta("Day01");std::cout << "\n\n";
	//aoc2015_02();clk.printDelta("Day02");std::cout << "\n\n";
	//aoc2015_03();clk.printDelta("Day03");std::cout << "\n\n";
	//aoc2015_04();clk.printDelta("Day04");std::cout << "\n\n";
	//aoc2015_05();clk.printDelta("Day05");std::cout << "\n\n";
	//aoc2015_06();clk.printDelta("Day06");std::cout << "\n\n";
	//aoc2015_07();clk.printDelta("Day07");std::cout << "\n\n";
	//std::cout << get_middle("test") << "\n"; // ==> "sihT si na !elpmaxe"
	//std::cout << get_middle("testing") << "\n"; // ==> "sihT si na !elpmaxe"
	//std::cout << get_middle("double  spaces") << "\n";// ==> "elbuod  secaps"")
	return 0;
}