#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <array>
#include <map>
#include <print>
#include <atomic>
#include <execution>

namespace {
	using I = std::int64_t;
	using S = std::string_view;
	using XY = std::tuple<I, I>;
	
	using Bounds = std::tuple<I, I, I, I>;
	inline auto outOfBounds = [](const XY& next, const Bounds& bounds)->bool {
		const auto& [x, y] = next;
		const auto& [l, t, r, b] = bounds;
		if (x < l || x >= r) { return true; }
		if (y < t || y >= b) { return true; }
		return false;
		};
}

void aoc2024_06()
{
	fs::path input(DataDir() / "2024_06.txt");
	TxtFile txt{ input };
	static Bounds bounds{};
	std::vector<S> grid;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		std::get<2>(bounds) = line.size();
		grid.emplace_back(line);
	}
	std::get<3>(bounds) = grid.size();
	
	


	enum class StepResult :int {
		DidStep,
		DidRotate,
		LeftGrid,
		InALoop,
	};
	struct Sentry {
		XY pos{};
		int dirIndex{0};
		std::array<XY, 4> protocol{
		XY{0,-1}, //-y
		XY{1,0}, //+x
		XY{0,1}, //+y
		XY{-1,0} //-x
		};
		XY obstacle{-1,-1};

		std::map<std::tuple<I, I>, I> visited{};
		StepResult step(const std::vector<S>& grid) {
			const auto& [x, y]=pos;
			const auto& [dx, dy] = protocol[dirIndex];
			XY next{ x + dx,y + dy };
			if (outOfBounds(next, bounds))
			{
				return StepResult::LeftGrid;
			}

			const auto& [newX, newY] = next;
			const char occ = grid[newY][newX];
			if (occ == '#' || next==obstacle ) {
				dirIndex = (dirIndex + 1) % protocol.size();
				return StepResult::DidRotate;
			}
			
			pos = next;
			I& visitCount=visited[pos]; // + pos has 4 entries, on 5 in a loop
			visitCount++;
			if (visitCount > 4) { return StepResult::InALoop; }
			return StepResult::DidStep;
		}
		void init(const std::vector<S>& grid, const Bounds& bounds) {
			visited.clear();
			const auto& [l, t, r, b] = bounds;
			for (I y = t; y < b; ++y)
			{
				for (I x = l; x < r; ++x)
				{
					if (grid[y][x] == '^') {
						dirIndex = 0;
						pos = { x,y };
						visited[pos]++;
						return;
					}
				}
			}
		}

		StepResult run(const std::vector<S>& grid) {
			StepResult stepResult = step(grid);
			while (stepResult != StepResult::LeftGrid && stepResult != StepResult::InALoop)
			{
				stepResult = step(grid);
			}
			return stepResult;
		}
	};

	Sentry guard;
	guard.init(grid, bounds);
	XY startPos = guard.pos;
	StepResult stepResult = guard.run(grid);
	
	I visitcount = guard.visited.size();
	std::println(std::cout, "visited {}", visitcount);

	std::vector<XY> path;
	for (const auto& [pos, count] : guard.visited)
	{
		if (pos != startPos)
		{
			path.emplace_back(pos);
		}
	}

	std::atomic<I> loopCount{};
	std::for_each(std::execution::par,
		path.cbegin(),path.cend(),[&](const XY& pos){
	//for (const XY& pos : path){
		Sentry guard;
		guard.pos = startPos;//possible enhancement: traverse path backwards, place and direction one step before collision
		guard.obstacle = pos;
		StepResult stepResult = guard.run(grid);
		
		if (stepResult == StepResult::InALoop) { loopCount++; }
	});

	std::println(std::cout, "loopCount {}", loopCount.load());
}

