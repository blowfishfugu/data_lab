#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <map>
#include <algorithm>
#include <vector>
#include <charconv>
#include <numeric>

namespace D13 {
	struct Shard {
		using Row = std::string;
		using Grid = std::vector<Row>;
		__int64 id{};
		__int64 u{};
		__int64 v{};
		Grid data{};
		Shard Transposed() {
			Shard t;
			t.id = id;
			t.data.resize(u);
			for (Row& dst : t.data)
			{
				dst.resize(v);
			}
			for (__int64 y = 0; y < v; ++y)
			{
				for (__int64 x = 0; x < u; ++x)
				{
					t.data[x][y] = data[y][x];
				}
			}
			return t;
		}
	};
	
	std::tuple<__int64, __int64> findMirror( Shard::Grid& data) {
			__int64 u = data.front().size();
			__int64 v = data.size();
			std::vector< std::tuple<__int64, __int64> > mirrors;
			for (__int64 y = 0; y < v; ++y)
			{
				for (__int64 last = (v-1); last > y; --last)
				{
					__int64 len = last - y;
					//if ( (len % 2) != 0) { continue; }
					__int64 center = y + ((len)/2);
					if (std::equal( data.begin()+y,
						data.begin()+last,
						data.rbegin()+ ((v)-last)
					
					))
					{
						std::cout << std::format("{}..{}..{} (len: {})\n", y, center, last, len);
						std::cout << std::format("     -> {} len:{}\n", center, len);
						mirrors.emplace_back(len, center);
					}
				}
			}

			if (mirrors.size() > 0)
			{
				std::sort(mirrors.begin(), mirrors.end());
				std::tuple<__int64, __int64> biggestSplit = mirrors.back();
				return biggestSplit;
			}
			return { 0,0 };
		}
}

void aoc2023_13()
{
	using namespace D13;
	fs::path input(DataDir() / "2023_13.txt");
	TxtFile txt{ input };

	std::vector<Shard> shards;

	auto tryAdd = [](std::vector<Shard>& shards, __int64 count, Shard&& current)
	{
		if (current.data.size() > 0) {
			current.id = count;
			current.u = current.data.front().size();
			current.v = current.data.size();
			shards.emplace_back(std::move(current));
		}
	};

	Shard current;
	__int64 count = 0LL;
	for ( auto& line : txt)
	{
		if (line.length() == 0) { 
			++count;
			tryAdd(shards, count, std::move(current));
			continue; 
		}
		current.data.emplace_back(line);
	}
	++count;
	tryAdd(shards, count, std::move(current));
	
	__int64 sum{};
	for (Shard& s : shards)
	{
		auto [wRow,rowIndex]=findMirror(s.data);
		std::cout << std::format("Shard {} : SplitRow {} with height {}\n", s.id, rowIndex,wRow);
		
		Shard t = s.Transposed();
		auto [wCol, colIndex] = findMirror(t.data);
		std::cout << std::format("ShardT{} : SplitCol {} with width {}\n",	t.id, colIndex, wCol);
		
		if (wRow > wCol) { colIndex = 0; }
		else { rowIndex = 0; }
		sum += (colIndex + rowIndex * 100);
		
	}

	std::cout << "sum: " << sum << "\n"; //..28286 too high!! .. 24238 too low 
	
}

