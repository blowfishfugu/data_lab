#include "DataDir.h"
#include <iostream>
#include <array>
#include <map>
#include <bitset>

constexpr const size_t IdCount = 'z' - 'a' + 1;
using bitmask = std::bitset<IdCount>;
using lookup = std::array<bitmask, IdCount>;

template<size_t packetLen>
__int64 findStart(std::string_view line, const lookup& Map)
{
	bitmask current{};
	for (int i = 0; i < packetLen && i<line.length(); ++i)
	{
		int insertindex = line[i] - 'a';
		current |= Map[insertindex];
	}
	for (int i = packetLen; i < line.length(); ++i)
	{
		if (current.count() == packetLen)
		{
			std::cout << "\n" << i << " " << current.count() << "\t" << current << "\n";
			return i;
		}
		
		current.reset();
		for (int active = (i - packetLen+1); active <= i; ++active)
		{
			int insertindex = line[active]-'a';
			current |= Map[insertindex];
			//std::cout << line[active];
		}
		//std::cout << "\n";
	}
	return 0;
}

lookup initIdentities()
{
	lookup bits;
	__int64 id = 1;
	for (char c = 'a'; c <= 'z'; ++c)
	{
		bitmask b = id;
		size_t index = c - 'a';
		//std::cout << c << "\t" << b << "\n";
		std::swap(bits[index], b);
		id *= 2;
	}
	return std::move(bits);
}

void aoc2022_06()
{
	lookup Map = initIdentities();

	fs::path input(DataDir() / "2022_06.txt");
	std::ifstream inFile(input);

	__int64 score = 0;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }

		score = findStart<4>(line, Map);
		std::cout << "Found 4:::::\t after:" << score << "\n";
		score = findStart<14>(line, Map);
		std::cout << "Found 14:::::\t after:" << score << "\n";
	}

}