#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <array>
#include <print>
#include <deque>
#include <set>

namespace {
	using I = std::int64_t;
	using SZ = std::tuple<I, I>;
	struct Dir {
		I dx{};
		I dy{};
		Dir(I x=0, I y=0) : dx{ x }, dy{ y } {}
	};

	static std::array<Dir, 4> Directions{
		Dir{ 0,-1}, //N
		Dir{ 1, 0}, //E
		Dir{ 0, 1}, //S
		Dir{-1, 0}, //W
	};

	struct Cell {
		I x{};
		I y{};
		I h{};
		Cell(I _x=0,I _y=0, I _h=0): x{_x},y{_y},h{_h}{ }
	};

	using Row = std::vector<Cell>;
	using Grid = std::vector<Row>;

	struct Trail {
		I id{};
		Cell* start{};
		I countOfNines{};
		I countOfPaths{};
		bool canEnter(const Cell* from, const Cell* to) {
			if ( (to->h - from->h) == 1) { return true; }
			return false;
		}

		void followTrail(const Grid& grid, const SZ& bounds, const Cell* start) {
			std::set<const Cell*> cells{};
			countOfNines = 0;
			countOfPaths = 0;
			std::deque<const Cell*> toVisit;
			toVisit.emplace_back(start);

			while (toVisit.size() > 0) {
				const Cell* visited = toVisit.front();
				toVisit.pop_front();

				if( visited->h==9) countOfPaths++;
				if (visited->h == 9 && !cells.contains(visited)) {
					countOfNines++;
				}
				cells.insert(visited);

				for (const Dir& dir : Directions) {
					I x = visited->x + dir.dx;
					I y = visited->y + dir.dy;
					if (x < 0 || x >= std::get<0>(bounds)) { continue; }
					if (y < 0 || y >= std::get<1>(bounds)) { continue; }

					const Cell* target = &grid[y][x];
					if (canEnter(visited, target)) {
						toVisit.push_back(target);
					}
				}
			}
		}
	};


	I char_to_height(char c) {
		if (c >= '0' && c <= '9') {
			return static_cast<I>(c) - '0';
		}
		return std::numeric_limits<I>::max(); //unreachable
	}
}

void aoc2024_10()
{
	fs::path input(DataDir() / "2024_10.txt");
	TxtFile txt{ input };
	
	Grid grid{};
	std::vector<Trail> trails{};
	I sy{};
	I sx{};
	I trailid{1};
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		if (sx == 0) { sx = line.length(); }
		Row row; row.resize(sx);
		I col{};
		for (const char c : line) {
			Cell& cell = row[col];
			cell.x = col;
			cell.y = sy;
			cell.h = char_to_height(c);
			++col;
			if (cell.h == 0) {
				trails.emplace_back(Trail{ trailid++,&cell,{} });
			}
		}
		grid.emplace_back(std::move(row));
		++sy;
	}
	
	I sumOfNines{};
	I sumOfPaths{};
	for (Trail& trail : trails) {
		trail.followTrail(grid, { sx,sy }, trail.start);
		//std::println(std::cout, "Trail {:>10}: {:>10}", trail.id, trail.countOfNines);
		sumOfNines += trail.countOfNines;
		sumOfPaths += trail.countOfPaths;

	}
	std::println(std::cout, "SumNines {:>8}: {:>10}", "", sumOfNines);
	std::println(std::cout, "SumPaths {:>8}: {:>10}", "", sumOfPaths);

	
}

