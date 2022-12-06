#include "DataDir.h"
#include <iostream>
#include <array>
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
#ifndef NDEBUG
			//std::cout << line[active];
#endif
		}
#ifndef NDEBUG
		//std::cout << "\n";
#endif
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
#ifndef NDEBUG
		//std::cout << c << "\t" << b << "\n";
#endif
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
		std::cout << "Found 4:::::: after: " << score << "\n";
		score = findStart<14>(line, Map);
		std::cout << "Found 14::::: after: " << score << "\n";
	}

}


//Im DebugModus schneller, im ReleaseBuild gewinnt das BitSet!
#include <intrin.h>
template<size_t packetLen>
__int64 findStart2(std::string_view line)
{
	using U64 = unsigned __int64;
	U64 current{};
	for (int i = 0; i < packetLen && i < line.length(); ++i)
	{
		current |= (1ULL << (line[i] - 'a'));
	}
	for (int i = packetLen; i < line.length(); ++i)
	{
		U64 bitCount = __popcnt64(current);
		if (bitCount == packetLen)
		{
			std::cout << "\n" << i << " " << bitCount << "\t" << current << "\n";
			return i;
		}

		current=0ULL;
		for (int active = (i - packetLen + 1); active <= i; ++active)
		{
			current |= (1ULL << (line[active] - 'a'));
		}
	}
	return 0;
}


void aoc2022_06_withInts()
{
	fs::path input(DataDir() / "2022_06.txt");
	std::ifstream inFile(input);

	__int64 score = 0;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }
		score = findStart2<4>(line);
		std::cout << "Found 4:::::: after: " << score << "\n";
		score = findStart2<14>(line);
		std::cout << "Found 14::::: after: " << score << "\n";
	}
}