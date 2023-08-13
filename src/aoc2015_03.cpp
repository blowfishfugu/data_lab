#include "DataDir.h"
#include <iostream>
#include <format>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>
#include <string_view>
#include <algorithm>
#include <functional>
#include <numeric>
#include <execution>

void aoc2015_03()
{
	fs::path input(DataDir() / "2015_03.txt");
	
	using coord = std::tuple<__int64, __int64>;
	struct infiniteGrid {
		std::map<coord, __int64> visited;
		void addVisit(const coord& c)
		{
			visited[c]++;
		}
		__int64 getVisited()
		{
			return visited.size();
		}
	};
	struct santa {
		infiniteGrid& houses;
		santa(infiniteGrid& g) : houses(g) {
			houses.addVisit(pos);
		}
		coord pos{}; //0,0
		void moveup() { std::get<1>(pos)--; houses.addVisit(pos);}
		void movedown() { std::get<1>(pos)++; houses.addVisit(pos);}
		void moveleft() { std::get<0>(pos)--; houses.addVisit(pos);}
		void moveright() { std::get<0>(pos)++; houses.addVisit(pos);}
		
	};
	
	infiniteGrid houses;
	santa s(houses);

	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0)
		{
			break;
		}

		for (const char c : line)
		{
			switch (c)
			{
			case('^'):
				s.moveup(); break;
			case('v'):
				s.movedown(); break;
			case('<'):
				s.moveleft(); break;
			case('>'):
				s.moveright(); break;
			default:
				break;
			}
		}
	}
	inFile.close();
	
	std::cout << std::format("{} visited\n", houses.getVisited());
	__int64 visited = houses.getVisited();
	assert(visited == 2081);

	infiniteGrid houses2;
	santa s1(houses2);
	santa s2(houses2);

	santa* currentSanta = &s1;
	std::ifstream inFile2(input);
	for (std::string line; std::getline(inFile2, line);)
	{
		if (line.length() == 0)
		{
			break;
		}

		for (const char c : line)
		{
			switch (c)
			{
			case('^'):
				currentSanta->moveup(); break;
			case('v'):
				currentSanta->movedown(); break;
			case('<'):
				currentSanta->moveleft(); break;
			case('>'):
				currentSanta->moveright(); break;
			default:
				break;
			}
			if (currentSanta == &s1) { currentSanta = &s2; }
			else if (currentSanta == &s2) { currentSanta = &s1; }
		}
	}
	inFile.close();

	std::cout << std::format("{} visited with robo\n", houses2.getVisited());
	__int64 visited2 = houses2.getVisited();
	assert(visited2== 2341);
}