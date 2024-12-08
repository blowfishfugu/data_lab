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

		V operator-(V o) const noexcept{
			return {x-o.x,y-o.y};
		}

		V operator+(V o) const noexcept{
			return { x + o.x,y + o.y };
		}

		V operator*(I scale) const noexcept{
			return { x * scale,y * scale };
		}
	};

	using Bounds = std::tuple<I, I, I, I>;
	auto inBounds(const V& v, const Bounds& bounds)->bool {
		const auto& [l, t, r, b] = bounds;
		if (v.x < l || v.x >= r) { return false; }
		if (v.y < t || v.y >= b) { return false; }
		return true;
	}

	using Antennas = std::vector<V>;
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
			if (c != '.' && c!='#') {
				AntennaMap[c].emplace_back(V{x,h});
			}
		}
		++h;
	}
	Bounds bounds{ 0,0,w,h };
	std::println(std::cout, "Bounds {}", bounds);
	std::println(std::cout, "{}\n", txt.buf);

	auto setMark = [](std::string& buf, const V at, const Bounds& bounds) {
		if (!inBounds(at, bounds)) { return; }
		const auto& [l, t, r, b] = bounds;
		const I bufStride = r + 1; // lines in buf have "\n"
		I pos = at.x + at.y * bufStride;
		if (buf[pos] == '#' && buf[pos]!='*') {
			buf[pos] = '*';
		}
		else {
			buf[pos] = '#';
		}
	};

	//directly place in txt-Buffer, stride is + the \n (or \r\n???), 
	// so bounds-check
	//setMark(txt.buf, { 0,0 }, bounds);
	//setMark(txt.buf, { w-1,0 }, bounds);
	//setMark(txt.buf, { 0,h-1 }, bounds);
	//setMark(txt.buf, { w-1,h-1 }, bounds);
	//std::println(std::cout, "{}\n", txt.buf);

	for (const auto& [name, antennas] : AntennaMap) {
		for (size_t i = 0; i < antennas.size(); ++i){
			const V& current = antennas[i];
			for (size_t j = i+1; j < antennas.size(); ++j) {
				const V& other = antennas[j];
				V toOther = other - current;
				V toCurrent = current-other;
				
				V mark1 = other + toOther;
				while (inBounds(mark1, bounds)) {
					setMark(txt.buf, mark1, bounds);
					mark1 = mark1 + toOther;
				}

				V mark2 = current+toCurrent;
				while (inBounds(mark2, bounds)) {
					setMark(txt.buf, mark2, bounds);
					mark2 = mark2 + toCurrent;
				}

				//"In fact, the three T - frequency antennas
				// are all exactly in line with two antennas, 
				// so they are all also antinodes!"
				setMark(txt.buf, current, bounds);
				setMark(txt.buf, other, bounds);
			}
		}
		std::println(std::cout, "{}\n", txt.buf);
	}

	I anodeCount = std::count_if(txt.buf.cbegin(), txt.buf.cend(),
		[](const char c) {return c == '#' || c == '*'; });
	std::println(std::cout, "Antinodes: {}\n", anodeCount);
}

