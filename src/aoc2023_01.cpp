#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <array>
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
		const __int64& val = numbers[static_cast<size_t>(line[i])];
		if (val != 0LL) { first = val; break; }
	}
	
	__int64 last{};
	for (size_t i = line.size()-1; i >=0; --i)
	{
		const __int64& val = numbers[static_cast<size_t>(line[i])];
		if (val != 0LL) { last = val; break; }
	}

	return first*10+last;
}

__int64 toCalibrate2(const std::string_view& line)
{
	static std::vector<std::string> strnumbers{ "one","two","three","four","five","six","seven","eight","nine" };
	std::vector<__int64> found;
	for (size_t i = 0; i < line.size(); ++i)
	{
		const __int64& val = numbers[static_cast<size_t>(line[i])];
		if (val != 0LL) { found.emplace_back(val); }
		else
		{
			std::string_view segment = line.substr(i);
			for (size_t val = 1; val <= strnumbers.size(); ++val)
			{
				if (segment.starts_with(strnumbers[val-1].c_str()) )
				{
					found.emplace_back(static_cast<__int64>(val));
					break;
				}
			}
		}
	}
	if (found.size() == 0) { return 0LL; }
	return found[0] * 10 + found[found.size() - 1];
}

void aoc2023_01()
{
	fs::path input(DataDir() / "2023_01.txt");
	TxtFile txt{ input };

	initNumbers();

	__int64 sum1 = 0LL;
	__int64 sum2 = 0LL;
	for (const auto& line : txt)
	{
		if (line.size() == 0) { break; }
		sum1+=toCalibrate(line);
		sum2 += toCalibrate2(line);
	}
	std::cout << "calibration:  " << sum1 << "\n";
	std::cout << "calibration2: " << sum2 << "\n";
	assert(sum1 == 55386);
	assert(sum2 == 54824);
}

