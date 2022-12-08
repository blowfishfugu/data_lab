#include "DataDir.h"
#include <iostream>
#include <array>
#include <algorithm>
#include <numeric>
#include <ppl.h>

void aoc2022_08()
{
	fs::path input(DataDir() / "2022_08.txt");
	std::ifstream inFile(input);

	struct Tree {
		char value = '\0';
		int visibleCount = 0;
		__int64 sceneScore = 0;
	};

	using Grid = std::vector<std::vector<Tree>>;
	Grid grid;
	__int64 wh = 0LL;

	auto initGrid = [](Grid& grid, __int64 wh)
	{
		grid.resize(wh);
		__int64 top = 0;
		__int64 bottom = grid.size() - 1;
		for (__int64 i = 0; i < wh; ++i)
		{
			grid[i].resize(wh);
		}
		//
		for (__int64 y = 0; y < wh; ++y)
		{
			std::vector<Tree>& row = grid[y];
			for (__int64 x = 0; x < wh; ++x)
			{
				Tree& current = row[x];
				if (y == 0) {
					current.visibleCount++;
				}
				if (y == wh - 1)
				{
					current.visibleCount++;
				}
				if (x == 0)
				{
					current.visibleCount++;
				}
				if (x == wh - 1)
				{
					current.visibleCount++;
				}
			}
		}
	};

	__int64 score = 0;
	__int64 y = 0;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }
		if (grid.size() == 0)
		{
			wh = line.length();
			initGrid(grid, wh);
		}
		for (__int64 x = 0; x < wh; ++x)
		{
			grid[y][x].value = line[x];
		}
		++y;
	}

	using Vec = std::tuple<__int64, __int64>;
	constexpr Vec directions[4] =
	{
		{0LL,1LL},
		{0LL,-1LL},
		{1LL,0LL},
		{-1LL,0LL},
	};
	auto checkVisibleTo = [](Grid& grid, Tree& current,
		__int64 y, __int64 x,
		const __int64 dy, const __int64 dx,
		const __int64 wh)->bool
	{
		y += dy;
		x += dx;
		__int64 viewDistance = 0;
		bool isVisible = true;
		while (y < wh && x < wh	&& y >= 0 && x >= 0
			)
		{
			viewDistance++; //größer oder nicht, baum ist sichtbar
			if (grid[y][x].value >= current.value)
			{
				isVisible = false;
				break;
			}
			y += dy;
			x += dx;
		}

		if (current.sceneScore == 0)
		{
			current.sceneScore = viewDistance;
		}
		else
		{
			current.sceneScore *= viewDistance;
		}

		return isVisible;
	};


	__int64 maxViewScore = 0;
	for (__int64 y = 0; y < wh; ++y)
	{
		for (__int64 x = 0; x < wh; ++x)
		{
			Tree& tree = grid[y][x];
			if (tree.visibleCount == 0)
			{
				for (const Vec& dir : directions)
				{
					__int64 dy = std::get<0>(dir);
					__int64 dx = std::get<1>(dir);
					if (checkVisibleTo(grid, tree, y, x, dy, dx, wh))
					{
						tree.visibleCount++;
						//kein early-break mehr wegen sceneScore
					}
				}
				maxViewScore = std::max(tree.sceneScore, maxViewScore);
			}
			if (tree.visibleCount > 0)
			{
				score++;
				//std::cout << tree.value;
			}
			else
			{
			//	std::cout << '.';
			}
		}
		//std::cout << "\n";
	}

	std::cout << "\nVisible:\t"<< score << "\n";
	std::cout << "ViewScore:\t" << maxViewScore << "\n";

}


void aoc2022_08_cleanup()
{
	fs::path input(DataDir() / "2022_08.txt");
	std::ifstream inFile(input);
	
	using Grid = std::vector<std::string>;
	Grid grid;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }
		if (grid.size() == 0) { grid.reserve(line.length()); }
		grid.emplace_back(line);
	}
	if (grid.size() == 0) { 
		return; 
	}
	__int64 wh = grid[0].length();

	struct Dir { __int64 dy = 0LL; __int64 dx = 0LL; };
	constexpr Dir directions[4] =
	{
		{0LL,1LL},
		{0LL,-1LL},
		{1LL,0LL},
		{-1LL,0LL},
	};
	auto checkVisibleTo = [](Grid& grid, char current,
		__int64 y, __int64 x,
		const Dir d,
		const __int64 wh)->std::tuple<bool,__int64>
	{
		y += d.dy;
		x += d.dx;
		__int64 viewDistance = 0;
		bool isVisible = true;
		while (y < wh && x < wh	&& y >= 0 && x >= 0
			)
		{
			viewDistance++; //größer oder nicht, baum ist sichtbar
			if (grid[y][x] >= current)
			{
				isVisible = false;
				break;
			}
			y += d.dy;
			x += d.dx;
		}
		return { isVisible,viewDistance };
	};

	std::vector<__int64> countVisibles;
	std::vector<__int64> maxViewScore;
	countVisibles.resize(wh);
	maxViewScore.resize(wh);
#ifndef NDEBUG
	concurrency::parallel_for(0LL,wh,[&](__int64 y) //<- bringt wiederrum nur was im debug-mode
#else
		for (__int64 y = 0; y < wh; ++y)
#endif
	{
		for (__int64 x = 0; x < wh; ++x)
		{
			char c = grid[y][x];
			bool foundVisible = false;
			__int64 sceneScore = 1;
			for (const Dir& dir : directions)
			{
				auto result = (checkVisibleTo(grid, c, y, x, dir, wh));
				foundVisible |= std::get<0>(result);
				sceneScore *= std::get<1>(result);
			}
			maxViewScore[y] = std::max(sceneScore, maxViewScore[y]);

			if (foundVisible)
			{
				countVisibles[y]++;
			}
		}
	}
#ifndef NDEBUG
		);
#endif
	
	__int64 sumVisibles = std::accumulate(countVisibles.begin(), countVisibles.end(), 0LL);
	__int64 viewScore = *std::max_element(maxViewScore.begin(), maxViewScore.end());
	std::cout << "\nVisible:\t" << sumVisibles << "\n";
	std::cout << "ViewScore:\t" << viewScore << "\n";

}