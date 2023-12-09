#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <array>

namespace D01
{

	using PosVal = std::tuple<size_t, __int64>;

	static std::array<char, 256> numbers{};
	namespace {
		void initNumbers() {
			numbers['1'] = 1LL;
			numbers['2'] = 2LL;
			numbers['3'] = 3LL;
			numbers['4'] = 4LL;
			numbers['5'] = 5LL;
			numbers['6'] = 6LL;
			numbers['7'] = 7LL;
			numbers['8'] = 8LL;
			numbers['9'] = 9LL;
		}
	}

	__int64 toCalibrate(const std::string_view& line)
	{
		__int64 first{};
		for (size_t i = 0; i < line.size(); ++i)
		{
			first = numbers[static_cast<size_t>(line[i])];
			if (first != 0LL) { break; }
		}

		__int64 last{};
		for (size_t i = line.size() - 1; i >= 0; --i)
		{
			last = numbers[static_cast<size_t>(line[i])];
			if (last != 0LL) { break; }
		}

		return first * 10 + last;
	}

	__int64 toCalibrate2(const std::string_view& line)
	{
		constexpr static std::array<std::string_view, 9> strnumbers{
			"one","two","three",
			"four","five","six",
			"seven","eight","nine"
		};

		auto toNumber = [](std::string_view segment)
		{
			const __int64& val = numbers[static_cast<size_t>(segment[0])];
			if (val != 0LL) { return val; }

			for (size_t val = 1; val <= strnumbers.size(); ++val)
			{
				if (segment.starts_with(strnumbers[val - 1]))
				{
					return (static_cast<__int64>(val));
				}
			}
			return 0LL;
		};

		__int64 first{};
		for (size_t i=0; i < line.size(); ++i)
		{
			first = toNumber(line.substr(i));
			if (first != 0LL) { break; }
		}

		__int64 last{};
		for (size_t i = line.size() - 1; i >= 0; --i)
		{
			last = toNumber(line.substr(i));
			if (last != 0LL) { break; }
		}
		
		return first * 10 + last;
	}
}

void aoc2023_01()
{
	using namespace D01;
	fs::path input(DataDir() / "2023_01.txt");
	TxtFile txt{ input };

	initNumbers();
	__int64 count{};
	__int64 sum1{};
	__int64 sum2{};
	for (const auto& line : txt)
	{
		if (line.size() == 0) { break; }
		++count;
		sum1 += toCalibrate(line);
		sum2 += toCalibrate2(line);
	}
	std::cout << "count: " << count << "\n";
	std::cout << "calibration:  " << sum1 << "\n";
	std::cout << "calibration2: " << sum2 << "\n";
	assert(sum1 == 55386);
	assert(sum2 == 54824);
}

