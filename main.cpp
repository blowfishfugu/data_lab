#include <Windows.h>
#include <array>
#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2024.h"

extern void fetchCpuInfos(std::ostream& outstream = std::cout);


#include <concepts>
#include <bitset>
#include <cassert>
#include <numeric>
#include <variant>

template<std::integral intType>
constexpr [[nodiscard]] size_t popc(const intType v) {
	if constexpr(std::is_signed_v<intType>){
		using uType = std::make_unsigned<intType>::type;
		uType uVal = (uType)v;
		std::bitset<sizeof(uVal) * 8> cc(uVal);
		return cc.count();
	}
	else{
		std::bitset<sizeof(v) * 8> cc(v);
		return cc.count();
	}
}

constexpr bool areAna(std::string_view& l, std::string_view& r) {
	if (l.size() != r.size()) { return false; }
	std::array<int, 0xFF> cl{};
	std::array<int, 0xFF> cr{};
	for (int i = 0; i < l.size(); ++i) {
		++cl[l[i]]; ++cr[r[i]];
	}
	return cl == cr;
}

void reverse(std::string& str) {
	if (str.length() == 0) { return; }
	size_t mid = std::midpoint(0ull, str.length());
	for (size_t l = 0ull, r = str.length() - 1; r >= mid && r>l; ++l,--r) {
		std::println(std::cout, "{}", str);
		std::println(std::cout, "{:<{}}l{:<{}}r", "", l, "", r-l-1 );
		std::swap(str[l], str[r]);
		std::println(std::cout, "{}", str);
	}
}

void fizzbuzz() {
	__int64 fizzed = 0;
	__int64 buzzed = 0;
	constexpr __int64 i3 = 3;
	constexpr __int64 i5 = 5;
	constexpr __int64 iProg = 1023ll * 1023ll * 1023ll*8ll;

	for (__int64 i = 0; i >=0; i+=3)
	{
		if ((i % i3) == 0) { ++fizzed; };
		if ((i % i5) == 0) { ++buzzed; };
		if ((i % iProg) == 0) { std::cout << std::format("{}\n", i); };
	}
	std::println(std::cout, "overall: {} fizzed, {} buzzed", fizzed, buzzed);
}

std::locale de_DE("de_DE");

auto toUpper = [](std::string& str) {
	for (char& c : str) { c = std::toupper(c, de_DE); }
	};

int main(int argc, char** argv)
{
	//fizzbuzz();
	//fetchCpuInfos(std::cout);
	//constexpr size_t pop1 = popc(0x1101LL);
	//constexpr size_t pop2 = popc(0x1101ULL);
	std::string toRev{ "abcdIefgh" };
	reverse(toRev);
	std::reverse(toRev.begin(), toRev.end());
	std::cout << toRev << "\n";
	
	toUpper(toRev);
	std::cout << toRev << "\n";
	return 0;

	if (argc > 2) {
		std::string_view v1{ argv[1] };
		std::string_view v2{ argv[2] };
		if (areAna(v1, v2)) {
			std::println(std::cout, "{} {} are anagram", v1, v2);
			return 0;
		}
	}
	return 0;

	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0], "2024");//Set

	StopWatch clk;
    
	//aoc2024_01(); clk.printDelta("Day01");std::cout<<"\n\n";
	//aoc2024_02(); clk.printDelta("Day02");std::cout<<"\n\n";
	////#aoc2015_19(); clk.printDelta("Day19");std::cout<<"\n\n";
	//aoc2024_03(); clk.printDelta("Day03");std::cout<<"\n\n";
	//aoc2024_04(); clk.printDelta("Day04");std::cout<<"\n\n";
	//aoc2024_05(); clk.printDelta("Day05");std::cout<<"\n\n";
	//aoc2024_06(); clk.printDelta("Day06");std::cout<<"\n\n";
	//aoc2024_07(); clk.printDelta("Day07");std::cout<<"\n\n";
	//aoc2024_08(); clk.printDelta("Day08");std::cout<<"\n\n";
	//aoc2024_09(); clk.printDelta("Day09");std::cout<<"\n\n";
	//aoc2024_10(); clk.printDelta("Day10");std::cout<<"\n\n";
	//aoc2024_11(); clk.printDelta("Day11");std::cout<<"\n\n";
	//aoc2024_12(); clk.printDelta("Day12");std::cout<<"\n\n";
	////#aoc2024_13(); clk.printDelta("Day13");std::cout<<"\n\n";
	//aoc2024_14(); clk.printDelta("Day14");std::cout<<"\n\n";
	//aoc2024_15(); clk.printDelta("Day15");std::cout<<"\n\n";
	////##aoc2024_16(); clk.printDelta("Day16");std::cout<<"\n\n";
	////##aoc2024_17(); clk.printDelta("Day17");std::cout<<"\n\n";
	////##aoc2024_18(); clk.printDelta("Day18");std::cout<<"\n\n";
	////##aoc2024_19(); clk.printDelta("Day19");std::cout<<"\n\n";
	////##aoc2024_20(); clk.printDelta("Day20");std::cout<<"\n\n";
	////##aoc2024_21(); clk.printDelta("Day21");std::cout<<"\n\n";
	//aoc2024_22(); clk.printDelta("Day22");std::cout<<"\n\n";
	//aoc2024_23(); clk.printDelta("Day23");std::cout<<"\n\n";
	//aoc2024_24(); clk.printDelta("Day24");std::cout<<"\n\n";
	//aoc2024_25(); clk.printDelta("Day25");std::cout<<"\n\n";
	return 0;
}