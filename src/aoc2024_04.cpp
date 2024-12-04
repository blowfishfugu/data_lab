#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <print>
#include <array>


void aoc2024_04()
{
	fs::path input(DataDir() / "2024_04.txt");
	TxtFile txt{ input };
	using SV = std::string_view;
	using I = std::int64_t;
	using Pos = std::tuple<I, I>;
	using Direction = std::tuple<I, I>;
	using Bounds = std::tuple<I, I>;
	
	std::vector<SV> grid{};
	Bounds gridSize{};
	auto& [w, h] = gridSize;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		grid.emplace_back(line);
		++h;
		w = line.size();
	}
	std::println(std::cout, "colcount: {} , rowcount: {}", w, h );
	
	constexpr std::array<Direction, 8> ScanDirections{
		Direction{ 1, 0},
		Direction{-1, 0},
		Direction{ 0,-1},
		Direction{ 0, 1},
		Direction{-1,-1},
		Direction{ 1,-1},
		Direction{-1, 1},
		Direction{ 1, 1}
	};

	static auto inGrid = [&w, &h](const Pos& p)->bool {
		return (
			std::get<0>(p) >= 0 && std::get<0>(p) < w
			&& std::get<1>(p) >= 0 && std::get<1>(p) < h
			);
		};

	static auto at = [&grid](const Pos& p)->char {
		const auto& [x, y] = p;
		return grid[y][x];
		};

	auto scan = [](const std::vector<SV>& grid, const Bounds& gridSize, const Direction& step)->I {
		I count{};
		const auto& [w, h] = gridSize;
		const auto& [dx, dy] = step;
		for (I y = 0; y < h; ++y)
		{
			for (I x = 0; x < w; ++x)
			{
				Pos xPos{ x,y };
				char c = at(xPos);
				if (c == 'X') {
					Pos sPos{ x + (dx * 3),y + (dy * 3) };
					if (inGrid(sPos))
					{
						Pos aPos{ x + (dx * 2),y + (dy * 2) };
						Pos mPos{ x + (dx * 1),y + (dy * 1) };
						if (at(sPos) == 'S' && at(aPos) == 'A' && at(mPos) == 'M')
						{
							++count;
						}
					}
				}
			}
		}
		return count;
	};

	I xCount{};
	for (const Direction& step : ScanDirections) {
		xCount += scan(grid, gridSize, step);
	}
	std::println(std::cout, "XMAS-Count: {}", xCount );

	auto scanMas = [](const std::vector<SV>& grid, const Bounds& gridSize)->I {
		I count{};
		using Pairs = std::array<Direction,2>; // X
		Pairs tl{
			Direction{ 1, 1 },
			Direction{ -1,-1 }
		};
		Pairs bl{
			Direction{ -1, 1 },
			Direction{ 1,-1 }
		};

		auto isValidPair = [&grid](const Pairs& pair, const Pos& aPos)->bool {
			std::array<char, 2> result{ '.','.' };
			const auto& [x, y] = aPos;
			for (size_t i = 0; i < pair.size();++i) {
				const auto& [dx, dy] = pair[i];
				Pos ms{ x + dx,y + dy };
				result[i] = at(ms);
			}
			return (
				(result[0] == 'M' && result[1] == 'S') ||
				(result[0] == 'S' && result[1] == 'M')
				);
		};

		const auto& [w, h] = gridSize;
		const I h1 = h - 1;
		const I w1 = w - 1;
		for (I y = 1; y < h1; ++y)
		{
			for (I x = 1; x < w1; ++x)
			{
				Pos aPos{ x,y };
				char c = at(aPos);
				if( c=='A')
				{
					if (isValidPair(tl, aPos) && isValidPair(bl, aPos))
					{
						++count;
					}
				}
			}
		}

		return count;
	};
	I XCount=scanMas(grid, gridSize);
	std::println(std::cout, "X-MAS-Count: {}", XCount );
}

