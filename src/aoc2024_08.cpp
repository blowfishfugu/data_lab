#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <map>
#include <tuple>
#include <algorithm>
#include <execution>

namespace {
	using I = std::int64_t;
	struct V {
		I x{};
		I y{};
		V operator-(V o) const noexcept { return { x - o.x,y - o.y }; }
		V operator+(V o) const noexcept { return { x + o.x,y + o.y }; }
	};

	using Bounds = std::tuple<I, I, I, I>;
	auto inBounds(const V& v, const Bounds& bounds)->bool {
		const auto& [l, t, r, b] = bounds;
		if (v.x < l || v.x >= r) { return false; }
		if (v.y < t || v.y >= b) { return false; }
		return true;
	}

	using Antennas = std::vector<V>;

	template<char mark>
	void setMark(std::string& buf, const V at, const Bounds& bounds) {
		const auto& [l, t, r, b] = bounds;
		const I bufStride = r + 1; // lines in buf have "\n"
		I pos = at.x + at.y * bufStride;
		if constexpr (mark == '*') {
			if (buf[pos] != '#') { //part2 shall not overwrite part1s results
				buf[pos] = mark;
			}
		}
		else
		{
			buf[pos] = mark;
		}
	};
}

void aoc2024_08()
{
	fs::path input(DataDir() / "2024_08.txt");
	TxtFile txt{ input };
	std::map<char, Antennas> AntennaMap;

	I w{};
	I h{};
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		w = line.length();
		for (I x = 0; x < w; ++x) {
			char c = line[x];
			if (c != '.') {
				AntennaMap[c].emplace_back(V{x,h});
			}
		}
		++h;
	}
	Bounds bounds{ 0,0,w,h };
	std::println(std::cout, "Bounds {}", bounds);
	//std::println(std::cout, "{}\n", txt.buf);

	auto expand = [&txt, &bounds](V start, const V& dir) {
		V mark1 = start + dir;
		if (inBounds(mark1, bounds)){ //part1, direct antinodes
			setMark<'#'>(txt.buf, mark1, bounds);
			mark1 = mark1 + dir;
		}

		//part2, increment on until boundary
		while(inBounds(mark1, bounds)) {
			setMark<'*'>(txt.buf, mark1, bounds);
			mark1 = mark1 + dir;
		}
	};

	for (const auto& [name, antennas] : AntennaMap) {
		for (size_t i = 0; i < antennas.size(); ++i) {
			const V& current = antennas[i];
			for (size_t j = i+1; j < antennas.size(); ++j) {
				const V& other = antennas[j];
				V toOther = other - current;
				V toCurrent = current - other;
				expand(other, toOther);
				expand(current, toCurrent);
				
				//"In fact, the three T - frequency antennas are all exactly 
				// in line with two antennas, so they are all also antinodes!"
				setMark<'*'>(txt.buf, current, bounds);
				setMark<'*'>(txt.buf, other, bounds);
			}
		}
	}
	std::println(std::cout, "{}\n", txt.buf);

	I anodeCount{};
	I morenodeCount{};
	for (const char c : txt.buf) {
		if (c == '#') {
			++anodeCount; ++morenodeCount;
		}
		else if (c == '*') {
			++morenodeCount;
		}
	}
	std::println(std::cout, "Antinodes: {}\n", anodeCount);
	std::println(std::cout, "Expanded Antinodes: {}\n", morenodeCount);
}

