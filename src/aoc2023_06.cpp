#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include "InputConversions.h"
#include <map>

void aoc2023_06()
{
	fs::path input(DataDir() / "2023_06.txt");
	TxtFile txt{ input };

	std::vector<__int64> Times;
	std::vector<__int64> Distances;
	__int64 count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		if( line.starts_with( "Time:" ) ) {
			Times = intSplit<' ', __int64, true>( line.substr(5) );
			++count;
		}
		else if( line.starts_with( "Distance:" ) )
		{
			Distances = intSplit<' ', __int64, true>( line.substr(9) );
			++count;
		}
	}
	std::cout << "count: " << count << "\n";
	assert(count == 2);
	assert( Times.size() == Distances.size() );

	__int64 maxTime= *std::max_element( Times.begin(), Times.end() );
	
	std::map<__int64, std::vector<__int64> > distsPerGiven;
	for( __int64 timeGiven : Times )
	{
		std::vector<__int64>& distResults = distsPerGiven[ timeGiven ];
		for( __int64 pressTime = 1; pressTime < timeGiven; ++pressTime )
		{
			__int64 speed = pressTime;
			__int64 remaining = timeGiven - pressTime;
			__int64 Distance = speed * remaining;
			distResults.emplace_back( Distance );
		}
	}

	std::vector<__int64> ways;
	distsPerGiven[ Times[ 0 ] ].reserve( Times[ 0 ] );
	for( size_t i=0;i<Times.size()&&i<Distances.size();++i)
	{
		__int64 timeGiven = Times[ i ];
		__int64 distanceCap = Distances[ i ];
		std::vector<__int64>& distResults = distsPerGiven[ timeGiven ];
		__int64 waysToWin=std::count_if( distResults.begin(), distResults.end(),
										 [ distanceCap ]( __int64 dist ) { return dist > distanceCap; } 
		);
		ways.emplace_back( waysToWin );
		std::cout << std::format("TimeGiven: {:>8} | Record: {:>15} | WaysToWin: {:>8}\n", timeGiven, distanceCap, waysToWin);
	}

	__int64 margin = 1;
	for (size_t i = 0; i < ways.size() - 1;++i ) {
		margin *= ways[i];
	}
	std::cout << "prerunmargin " << margin << "\n";
	std::cout << "margin " << ways[ways.size()-1] << "\n";
}

