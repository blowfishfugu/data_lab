#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <map>
#include <algorithm>
#include <numeric>
namespace D11
{
	struct Galaxy
	{
		__int64 id{};
		__int64 x{};
		__int64 y{};
		__int64 mapX{};
		__int64 mapY{};
	};

	__int64 dist( const Galaxy& l, const Galaxy& r )
	{
		return std::abs( r.x - l.x ) + std::abs( r.y - l.y );
	}
}

void aoc2023_11()
{
	using namespace D11;
	fs::path input( DataDir() / "2023_11.txt" );
	TxtFile txt{ input };

	__int64 count = 0LL;
	std::vector<std::string_view> lines;
	for( const auto& line : txt )
	{
		if( line.length() == 0 ) { break; }
		lines.push_back( line );
		++count;
	}

	std::cout << "count: " << count << "\n";
	if( lines.size() == 0 ) { return; }

	std::vector<__int64> galaxiesX; galaxiesX.resize( lines.front().length() );
	std::vector<__int64> galaxiesY; galaxiesY.resize( lines.size() );


	std::vector<Galaxy> Gs;
	__int64 id{ 1 };
	for( size_t y = 0; y < lines.size(); ++y )
	{
		const auto& row = lines[ y ];
		for( size_t x = 0; x < row.size(); ++x )
		{
			char c = row[ x ];
			if( c == '#' ) {
				Galaxy g;
				g.mapX = x;
				g.mapY = y;
				galaxiesX[ x ]++;
				galaxiesY[ y ]++;
				g.id = id++;
				Gs.emplace_back( g );
			}
		}
	}

	constexpr __int64 Space = 1'000'000;
	std::vector<__int64> scalesX; scalesX.resize( lines.front().length() );
	for( size_t i = 0; i < scalesX.size(); ++i ) {
		if( galaxiesX[ i ] == 0 ) { scalesX[ i ] = Space; }
		else { scalesX[ i ] = 1; }
	}

	std::vector<__int64> scalesY; scalesY.resize( lines.size() );
	for( size_t i = 0; i < scalesY.size(); ++i ) {
		if( galaxiesY[ i ] == 0 ) { scalesY[ i ] = Space; }
		else { scalesY[ i ] = 1; }
	}

	std::partial_sum( scalesX.begin(), scalesX.end(), scalesX.begin() );
	std::partial_sum( scalesY.begin(), scalesY.end(), scalesY.begin() );

	for( Galaxy& g : Gs )
	{
		g.x = scalesX[ g.mapX ] - 1;
		g.y = scalesY[ g.mapY ] - 1;
	}

	std::vector<__int64> minimums;
	for( size_t i = 0; i < Gs.size() - 1; ++i )
	{
		const Galaxy& current = Gs[ i ];
		for( size_t o = i + 1; o < Gs.size(); ++o )
		{
			const Galaxy& other = Gs[ o ];
			minimums.push_back( dist( current, other ) );
		}
	}

	__int64 sum{};
	for( __int64 c : minimums ) { 
		sum += c; 
	}
	std::cout << sum << "\n";
}

