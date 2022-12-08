#include "DataDir.h"
#include <iostream>
#include <array>

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
		__int64 dy, __int64 dx,
		__int64 wh)->bool
	{
		y += dy;
		x += dx;
		__int64 viewDistance = 0;
		bool isVisible = true;
		while (y < wh && x < wh
			&& y >= 0 && x >= 0
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
					}
				}
				maxViewScore = std::max(tree.sceneScore, maxViewScore);
			}
			if (tree.visibleCount > 0)
			{
				score++;
				std::cout << tree.value;
			}
			else
			{
				std::cout << '.';
			}
		}
		std::cout << "\n";
	}

	std::cout << "\nVisible:\t"<< score << "\n";
	std::cout << "ViewScore:\t" << maxViewScore << "\n";

}