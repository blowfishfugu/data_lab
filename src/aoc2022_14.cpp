#include "DataDir.h"
#include <iostream>
#include <sstream>
#include <map>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <functional>

#include <ppl.h>
#include <concrt.h>

using Point = std::tuple<__int64, __int64>;
using Direction = std::tuple<__int64, __int64>;
struct SandCave{
	//might be rock=1, air=0, othersand=2
	std::map<Point, __int64> occupied; //<- hieraus statische bitmap machen? y*w+x ...
	__int64 left = std::numeric_limits<__int64>::max();
	__int64 right = std::numeric_limits<__int64>::min();
	__int64 top = 0LL;
	__int64 bottom = std::numeric_limits<__int64>::min();
	void updateBounds( const Point& pt ){
		__int64 x = std::get<0>( pt );
		__int64 y = std::get<1>( pt );
		left = std::min( x, left );
		right = std::max( x, right );
		top = std::min( y, top );
		bottom = std::max( y, bottom );
	}

	void addRock( Point pt ){
		updateBounds( pt );
		occupied[ pt ] = 1;
	}

	void addSand( Point pt ){
		updateBounds(pt);
		occupied[ pt ] = 2;
	}

	void addStripe(Point s, Point e)
	{
		__int64 stepX = std::get<0>(e) - std::get<0>(s);
		__int64 stepY = std::get<1>(e) - std::get<1>(s);
		if (stepX != 0) { stepX = stepX / std::abs(stepX); }
		if (stepY != 0) { stepY = stepY / std::abs(stepY); }
		while (s != e)
		{
			addRock(s);
			std::get<0>(s) += stepX;
			std::get<1>(s) += stepY;
		}
		addRock(e);
	}

	__int64 HitTest( Point pt ){
		
		//part2, floor on -2
		__int64 y = std::get<1>(pt);
		if (y >= bottom)
		{
			return 1;
		}

		const auto& tst = occupied.find( pt );
		if( tst!=occupied.end() )
		{ 
			return tst->second;
		}
		return 0;
	}

	bool MoveMark(Point& mark)
	{
		if (HitTest(mark) != 0) //start oder zu weit
		{
			return false;
		}
		Point testBottom = mark;
		std::get<1>(testBottom)++;
		if (HitTest(testBottom) == 0)
		{
			mark = testBottom;
			return true;
		}
		std::get<0>(testBottom)--;
		if (HitTest(testBottom) == 0)
		{
			mark = testBottom;
			return true;
		}
		std::get<0>(testBottom)+=2;
		if (HitTest(testBottom) == 0)
		{
			mark = testBottom;
			return true;
		}
		addSand(mark);
		mark = { 500,0 };
		return true;
	}

	void print(const Point& mark)
	{
		if (occupied.size() == 0) { return; }
		__int64 w = right - left;
		__int64 h = bottom - top;
#ifdef NDEBUG
		std::string output;
		output.reserve((w+4)*(h+2));
		//__int64 mTop = std::get<1>(mark); //0
		for( __int64 y=-1; y<=bottom+1;++y)
		{ 
			for( __int64 x=left-2;x<=right+2;++x)
			{ 
				char c = '.';
				Point pos = std::tie(x, y);
				if (pos == mark) { c = 'x'; }
				else
				{
					__int64 tst = HitTest(pos);
					if (tst == 0) { c = '.'; }
					else if (tst == 1) { c = '#'; }
					else if (tst == 2) { c = 'o'; }
				}
				output.push_back(c);
			}
			output.push_back('\n');
		}
		std::cout << output.c_str() <<"\n";
#endif
		std::cout << w << " x " << h << "\n";
	}
};

std::vector<Point> readPath(const std::string& line)
{
	std::vector<Point> path;
	std::string strPair;
	std::istringstream in(line);
	std::getline(in, strPair, ' ');
	while (strPair.length() > 0)
	{
			size_t comma=strPair.find(',');
			if (comma != std::string::npos)
			{
				std::string left = strPair.substr(0, comma);
				std::string right = strPair.substr(comma+1);
				path.emplace_back(std::stoll(left), std::stoll(right));
			}
		
		if (!std::getline(in, strPair, ' '))
		{
				break;
		}
	}
	return path;
}

void aoc2022_14()
{
	fs::path input(DataDir() / "2022_14.txt");
	std::ifstream inFile(input);
	
	SandCave grid;
	for( std::string line; std::getline( inFile, line );)
	{
		if( line.length() == 0 ) { break; }
		std::vector<Point> path = readPath(line);
		for (int i = 1; i < path.size(); ++i)
		{
			Point& s = path[i-1];
			Point& e = path[i];
			grid.addStripe(s, e);
		}
	}

	grid.bottom = grid.bottom += 2;

	Point mark{ 500,0 };
	grid.print(mark);
	__int64 stepCount = 1;
	while (std::get<1>(mark) <= grid.bottom)
	{
		if (!grid.MoveMark(mark)) 
		{ 
			//startpunkt besetzt
			break; 
		}
		if (stepCount % 1'000'000 == 0)
		{
			grid.print(mark);
		}
		stepCount++;
	}
	grid.print(mark);
	__int64 score=std::count_if(grid.occupied.cbegin(), grid.occupied.cend(),
		[](const std::map<Point, __int64>::value_type& pair) {
		return pair.second == 2;
		});
	//1133
	//27566
	std::cout << "SandCount= " << score << "\n";
}