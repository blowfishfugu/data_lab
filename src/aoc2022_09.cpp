#include "DataDir.h"
#include <iostream>
#include <array>
#include <map>
#include <sstream>
struct Vec 
{
	__int64 x = 0;
	__int64 y = 0;
	Vec& operator+=(const Vec& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}
	Vec& normalizeOffset()
	{
		if (x != 0) { x /= std::abs(x); }
		if (y != 0) { y /= std::abs(y); }
		return *this;
	}
#ifndef NDEBUG
	//check, Position von Head-Tail
	friend std::ostream& operator<<(std::ostream& os, const Vec& v)
	{
		os << " ( " << v.x << " , " << v.y << " ) ";
		return os;
	}
#endif
};

Vec operator-(const Vec& l, const Vec& r)
{
	return { l.x - r.x,l.y-r.y };
}

template<size_t Count>
struct Rope
{
	std::array<Vec, Count> Segments;
	Vec& H() { return Segments[0]; }
	Vec& T() { return Segments[Count - 1]; }
	
	std::map<std::tuple<__int64, __int64>, __int64> visited;
	
	void storeT()
	{
		Vec& t = T();
		visited[std::tie(t.x, t.y)]++;
	}
	
	void update(const Vec& dir)
	{
		H() += dir;
		for (size_t i = 1; i < Count; i++)
		{
			Vec& head = Segments[i - 1];
			Vec& trail = Segments[i];
			Vec dist = head - trail;
			if (std::abs(dist.x) > 1 || std::abs(dist.y) > 1)
			{
				trail += dist.normalizeOffset();
			}
		}
		storeT();
	}
};

void aoc2022_09()
{
	fs::path input(DataDir() / "2022_09.txt");
	std::ifstream inFile(input);
	
	struct Move
	{
		Vec dir{};
		__int64 steps = 0;
	};

	std::map<char, Vec> directions = {
		{'R', { 1LL, 0LL } },
		{'L', {-1LL, 0LL} },
		{'U', { 0LL, 1LL } },
		{'D', { 0LL,-1LL} },
	};

	auto buildMove = [&directions](std::string& line)
	{
		std::istringstream input(line);
		std::string dir; std::getline(input, dir, ' ');
		char key = dir[0];
		std::getline(input, dir);
		__int64 steps = std::stoll(dir);
		Move m{ directions[key], steps };
		return std::move(m);
	};

	std::vector<Move> moves;
	__int64 score = 0;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }
		moves.emplace_back(buildMove(line));
	}
	
	
	Rope<2> rope;
	rope.storeT();
	Rope<10> largerope;
	largerope.storeT(); //0,0
	for (const Move& move : moves)
	{
		//std::cout << "\n\n--------Move " << move.dir << " x " << move.steps << "\n";
		for (__int64 step = 0; step < move.steps; step++)
		{
			rope.update(move.dir);
			largerope.update(move.dir);
		}
	}
	//6266
	//2369
	std::cout << "T< 2> visited " << rope.visited.size() << "\n";
	std::cout << "T<10> visited " << largerope.visited.size() << "\n";
	
}