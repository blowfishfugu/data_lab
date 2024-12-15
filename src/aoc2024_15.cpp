#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <bitset>
#include <numeric>
#include <vector>
#include <map>
#include <print>
#include <sstream>
#include <chrono>
using namespace std::chrono_literals;

namespace Boxle{
	constexpr bool part2 = true;
	using I = std::int64_t;
	struct V {
		I x{};
		I y{};
		V operator-(V o) const noexcept { return { x - o.x,y - o.y }; }
		V operator+(V o) const noexcept { return { x + o.x,y + o.y }; }
		V operator*(I s) const noexcept {
			I x_ = x * s; I y_ = y * s; return V{ x_,y_ };
		}
		V& operator+=(V o) noexcept { x += o.x; y += o.y; return *this; };
		auto operator<=>(const V& r) const = default;
	};
	using Row = std::string;
	using Grid = std::vector<Row>;
	
	void clearScreen() {
		// CSI[2J clears screen, CSI[H moves the cursor to top-left corner
		std::cout << "\x1B[2J\x1B[H";
	}

	void print(const Grid& g, const V& move, const V& curPos) {
		std::ostringstream os;
		std::println(os, "Move x{:>2},y{:>2}:", move.x, move.y);
		std::println(os, "Pos@ x{:>2},y{:>2}:", curPos.x, curPos.y);
		for (const Row& r : g) {
			std::println(os, "{}", r);
		}
		std::print(os, "\n\n");
		clearScreen();
		std::cout << os.str();
		std::this_thread::sleep_for(1ms);
	}
	void captureBoxes(const Grid& g, const V& move, V& curPos, std::map<V,char>& boxes) {
		char current = g[curPos.y][curPos.x];
		//possible optimization -> directly break on seeing '#'
		if (current != '[' && current != ']') {
			return;
		}

		V otherPos{ curPos.x + 1,curPos.y };
		if (current == ']') {
			otherPos.x = curPos.x - 1;
		}
		char other = g[otherPos.y][otherPos.x];
		if (!boxes.contains(curPos)) {
			boxes.emplace(curPos, current);
			V nextPos = curPos + move;
			captureBoxes(g, move, nextPos, boxes);
		}
		if (!boxes.contains(otherPos)) {
			boxes.emplace(otherPos, other);
			V nextPos = otherPos + move;
			captureBoxes(g, move, nextPos, boxes);
		}
	}

	bool update(Grid& g, const V& move, V& curPos) {
		V nextPos = curPos + move;
		char& current = g[curPos.y][curPos.x];
		char& next = g[nextPos.y][nextPos.x];
		if (next == '#') {
			return false;
		}

		if constexpr (part2) {
			if (move.y == 0LL)
			{
				if (next == '[' || next == ']') {
					if (!update(g, move, nextPos)) {
						return false;
					}
				}
			}
			else {
				if (next == '[' || next == ']') {
					std::map<V, char> boxes;
					captureBoxes(g, move, nextPos, boxes);
					//..can move all boxes
					for (const auto& [b, val] : boxes) {
						V target = b + move;
						char tst = g[target.y][target.x];
						if (tst != '[' && tst != ']' && tst != '.' && tst != 'O') {
							return false;
						}
					}
					//..clearBoxes
					for (const auto& [b, val] : boxes) {
						g[b.y][b.x] = '.';
					}
					//..moveBoxes
					for (const auto& [b, val] : boxes) {
						V target = b + move;
						g[target.y][target.x] = val;
					}

					if (current == '@') {
						curPos = nextPos;
					}
					std::swap(next, current);
					return true;
				}
			}
		}

		if (next == 'O') {
			if (!update(g, move, nextPos)) {
				return false;
			}
		}
		if (next == '.') {
				if (current == '@') {
					curPos = nextPos;
				}
				std::swap(next, current);
			return true;
		}
		return false;
	}

	std::vector<V> getBoxPositions(const Grid& g, I w, I h) {
		std::vector<V> boxes{};
		for (I y=1; y < (h-1); ++y) {
			for (I x=1; x < (w-1); ++x) {
				if (g[y][x] == 'O' || g[y][x]=='[') {
					boxes.emplace_back(V{ x,y });
				}
			}
		}
		return boxes;
	}
}

void aoc2024_15()
{
	fs::path input(DataDir() / "2024_15.txt");
	TxtFile txt{ input };

	using I = Boxle::I;
	using V = Boxle::V;
	using Row = Boxle::Row;
	using Grid = Boxle::Grid;
	using Boxle::part2;

	std::map<char, V> Directions{
		{'<', V{-1, 0}},
		{'>', V{ 1, 0}},
		{'^', V{ 0,-1}},
		{'v', V{ 0, 1}}
	};

	Grid grid;
	std::vector<V> cmds;
	I w{};
	I h{};
	V curPos{};
	for (int readState{}; const auto& line : txt)
	{
		if (readState == 2) { break; }
		if (line.length() == 0) { readState++; }
		if (readState == 0) {
			Row row{};
			for (I x{}; char c : line) {
				if constexpr (part2) {
					if (c == 'O') { c = '['; }
				}
				row.push_back(c);
				if (c == '@') { curPos = { x,h }; }
				if constexpr (part2) {
					if( c=='#'){ row.push_back('#'); }
					if( c=='['){ row.push_back(']'); }
					if( c=='@'){ row.push_back('.'); }
					if( c=='.'){ row.push_back('.'); }
					++x;
				}
				++x;
			}
			w = row.length();
			grid.emplace_back(row);
			++h;
		}
		else if (readState == 1) {
			for (const char c : line) {
				cmds.emplace_back(Directions[c]);
			}
		}
	}
	
	print(grid, {0,0}, curPos);
	for (const V& move : cmds) {
		Boxle::update(grid, move, curPos);
//		print(grid, move, curPos);
	}
	print(grid, { 0,0 }, curPos);

	std::vector<V> boxes= Boxle::getBoxPositions(grid, w,h);
	I gpsSum = std::accumulate(boxes.cbegin(), boxes.cend(), 0LL,
		[](I cnt, const V& box) {
			I val = 100LL * box.y + box.x;
			return cnt + val;
		});
	std::println(std::cout, "gpsSum={}", gpsSum);
}

