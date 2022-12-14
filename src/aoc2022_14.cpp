#include "DataDir.h"
#include <iostream>
#include <map>
#include <array>
#include <algorithm>
#include <functional>

#include <ppl.h>
#include <concrt.h>

using Point = std::tuple<__int64, __int64>;

struct SandCave{
	//might be rock=1, air=0, othersand=2
	std::map<Point, __int64> occupied;
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
		updateBounds( pt );
		occupied[ pt ] = 2;
	}

	__int64 HitTest( Point pt ){
		const auto& tst = occupied.find( pt );
		if( tst!=occupied.end() )
		{ 
			return tst->second;
		}
		return 0;
	}
};

void aoc2022_14()
{
	fs::path input(DataDir() / "2022_14.txt");
	std::ifstream inFile(input);
	
	
	for( std::string line; std::getline( inFile, line );)
	{
		if( line.length() == 0 ) { break; }
	}
	

	
}