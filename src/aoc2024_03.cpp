#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <functional>
#include <print>
#include <bitset>
#include <concepts>

template<size_t SIZE, typename ... Args>
constexpr void buildBitSet(Args... args){
	std::bitset<SIZE> Digits{};
	(Digits.set(args), ...);
	std::println(std::cout, "constexpr std::bitset<{}> Digits{{\"{}\"}}", Digits.size(), Digits.to_string());
};
//buildBitSet<256>( '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' );
//bool isDigit(char c){
// constexpr std::bitset<256> Digits{"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111000000000000000000000000000000000000000000000000"};
// return Digits.test(c);
// }

void aoc2024_03()
{
	//buildBitSet<256>( '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' );
	fs::path input(DataDir() / "2024_03.txt");
	TxtFile txt{ input };
	
	//'is_digit'
	constexpr std::bitset<256> Digits{"0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001111111111000000000000000000000000000000000000000000000000"};
	using I = std::int64_t;
	
	constexpr std::string_view mul{ "mul(" };
	constexpr std::string_view does{ "do()" };
	constexpr std::string_view donts{ "don't()" };

	static bool enabled = true;
	struct Op {
		std::string_view cmd;
		I paramLeft{};
		I paramRight{};
		std::function<I(I, I)> func{};
		size_t foundAt{};
		auto operator<=>( Op& r) { return foundAt <=> r.foundAt; }
		auto operator<=>( const Op& r) { return foundAt <=> r.foundAt; }
	};


	static auto buildMul = [](I l, I r, size_t at)->Op {
		return { mul,l,r,[](I p1, I p2)->I { return enabled?p1 * p2:0LL; }, at };
		};

	static auto buildDo = [](I, I, size_t at)->Op {
		return { does,0,0, [](I,I)->I { enabled = true; return 0; }, at };
		};

	static auto buildDont = [](I, I, size_t at)->Op {
		return { donts,0,0, [](I,I)->I { enabled = false; return 0; }, at };
		};

	// alternative: regex?
	static auto consumeNumber = [](const std::string_view& line, size_t startOfDigit, int minLen, int maxLen)
		-> std::tuple<bool, size_t, I>
		{
			if (startOfDigit >= line.size())
			{
				return { false,startOfDigit,{} };
			}
			size_t endOfDigit = startOfDigit;
			size_t digitLen = 0LL;
			char c = line[endOfDigit];
			while (endOfDigit<line.size()-1 && Digits.test(c)) {
				++endOfDigit;
				++digitLen;
				c = line[endOfDigit];
				if (digitLen > maxLen)
				{
					return { false,startOfDigit,{} };
				}
			}
			
			if ( digitLen >= minLen && digitLen <= maxLen )
			{
				I p1 = toInt<I>(line.substr(startOfDigit, digitLen));
				return { true,endOfDigit,p1 };
			}
			return { false,startOfDigit,{} };
		};

	static auto consumeChar = [](const std::string_view& line, size_t& at, char tst)->bool {
		if (at >= line.size()) { return false; }
		char c = line[at];
		++at;
		return c == tst;
		};



	std::vector<Op> ops{};
	size_t globalPos{};
	for (const std::string_view& line : txt)
	{
		if (line.length() == 0) { break; }
		//std::println(std::cout, "{}", line);

		size_t foundDo = line.find(does, 0LL);
		while (foundDo != std::string_view::npos)
		{
			ops.emplace_back(buildDo(0, 0, globalPos + foundDo));
			foundDo = line.find(does, foundDo + does.size());
		}

		size_t foundDont = line.find(donts, 0LL);
		while (foundDont != std::string_view::npos)
		{
			ops.emplace_back(buildDont(0, 0, globalPos + foundDont));
			foundDont = line.find(donts, foundDont + donts.size());
		}

		size_t foundMul = line.find(mul, 0LL);
		while (foundMul != std::string_view::npos)
		{
			size_t startOfDigit = foundMul + mul.size();
			//const size_t inspectEnd = (foundMul + mul.size() + 9 >= line.size()) ? line.size()-foundMul : mul.size() + 9;
			//const std::string_view inspected{ &line[foundMul], inspectEnd };
			//std::println(std::cout, "inspect: '{}'", inspected);

			foundMul = line.find(mul, foundMul + mul.size());
			auto [p1valid, endOfDigit, p1] = consumeNumber(line, startOfDigit, 1, 3);
			if (!p1valid)
			{
				continue;
			}
			if (!consumeChar(line, endOfDigit, ','))
			{
				continue;
			}
			auto [p2valid, endOfDigit2, p2] = consumeNumber(line, endOfDigit, 1, 3);
			if (!p2valid)
			{
				continue;
			}
			
			if ( !consumeChar(line, endOfDigit2, ')') ) {
				continue;
			}

			if (p1valid && p2valid )
			{
				ops.emplace_back(buildMul(p1, p2, globalPos+startOfDigit));
			}
		}

		globalPos += line.size();
	}
	
	I sum{};
	std::sort(ops.begin(), ops.end());
	for (const Op& op : ops)
	{
		sum += op.func(op.paramLeft, op.paramRight);
	}
	std::println(std::cout, "Sum of muls: {}", sum);
}

