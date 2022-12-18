#include "DataDir.h"
#include <iostream>
#include <array>
#include <deque>
#include <map>

constexpr __int64 chamberWidth = 7;
/*
3..@@@@.|
2.......|
1.......|
0.......|
+76543210
//top=-1 floor=-1 ->height=top+1 =0
//startx=0+2
//bottomOfNew=top+4;
*/

using rowType = std::uint8_t;
struct Rock
{
	std::array<rowType, 4> rows{};
	int rockIndex = 0;
};

constexpr rowType floorRow = 0b11111111;
constexpr rowType insertRow = 0b00000001;
constexpr rowType emptyRow = ~insertRow;
using Chamber = std::deque<rowType>;

void initRocks(std::array<Rock, 5>& data)
{
	data[0].rows = {
		0b00000000,
		0b00000000,
		0b00000000,
		0b00111100,
	};
	data[0].rockIndex = 0;

	data[1].rows = {
		0b00000000,
		0b00010000,
		0b00111000,
		0b00010000,
	};
	data[1].rockIndex = 1;

	data[2].rows = {
		0b00000000,
		0b00001000,
		0b00001000,
		0b00111000,
	};
	data[2].rockIndex = 2;

	data[3].rows = {
		0b00100000,
		0b00100000,
		0b00100000,
		0b00100000,
	};
	data[3].rockIndex = 3;

	data[4].rows = {
		0b00000000,
		0b00000000,
		0b00110000,
		0b00110000,
	};
	data[4].rockIndex = 4;
}

Rock nextRock(const std::array<Rock,5>& data)
{
	static size_t current = 0LL;
	if (current >= data.size()) { 
		current = 0LL; 
	}
	return data[current++];
}

std::tuple<char,__int64> nextJet(const std::string& jets)
{
	static size_t current = 0LL;
	if (current >= jets.size()) 
	{ 
		current = 0LL; 
	}
	__int64 sequenceIndex = current;
	return { jets[current++],sequenceIndex };
}

__int64 top(const Chamber& c)
{
	for (__int64 i = c.size() - 1; i >= 0; --i)
	{
		if (c[i] != emptyRow)
		{
			return i;
		}
	}
	return 0;
}

void adjustChamberHeight(Chamber& ch, __int64 rowCount)
{
	while (ch.size() < rowCount)
	{
		ch.emplace_back(emptyRow);
	}
}

void printRow(const rowType& row, char onset = '.', char onempty = '#')
{
	for (int i = 7; i >= 0; --i)
	{
		rowType tst = (row >> i) & 0x01;
		std::cout << ((tst) ? onset : onempty);
	}
	std::cout << "\n";
}

void printChamber(const Chamber& chamber)
{
	for (int i = chamber.size() - 1; i >= 0; i--)
	{
		printRow(chamber[i]);
	}
	std::cout << "\n\n";
}

/* place row
0001000 1 //...@...|
1101100 0 //..#..##|
-------
1100100 0 //..#@.##|

0001000 1
0010011 1 | //invrow bin_or row
----------
0011011 1
1100100 0 //inv back //..#@.##|
*/
bool mergeRow(Chamber& c, rowType row, __int64 index)
{
	static rowType fillCheck = 0b00000000;
	adjustChamberHeight(c, index+1);
	rowType& target = c[index];
	
	target= ~target | (row|insertRow);
	fillCheck |= target;
	target = ~target;
	if (fillCheck == 0b11111111)
	{
		fillCheck = 0b00000000;
		return true;
	}
	return false;
}

__int64 recalcChamber(Chamber& chamber, __int64 cycle, __int64 rockIndex, __int64 sequenceIndex, __int64 currentHeight)
{
	static std::map<std::tuple<std::string,__int64,__int64,__int64>, __int64> chamberPattern;
	if (chamber.size() < 5) { return 0; }
	__int64 index = 1;
	rowType target = chamber[index];
	target = ~target | insertRow;
	while( target!=0b11111111 && index<chamber.size()-8 )
	{ 
		target |= ~chamber[index];
		index++;
	}
	if (target==0b11111111 && index>1)
	{
		__int64 oldsize = chamber.size();
		chamber.erase(chamber.begin()+1, chamber.begin() + (index-1));
		return oldsize - chamber.size();
	}
	return 0;

	std::string pattern;
	for (const char& k : chamber)
	{
		pattern.push_back(k);
	}
	__int64 height = top(chamber);
	auto key = std::tie(pattern, rockIndex, sequenceIndex,height);
	__int64 lastSeen = chamberPattern[key];
	if (lastSeen == 0)
	{
		chamberPattern[key] = height;
	}
	else
	{
		if ( height == lastSeen)
		{
			std::cout << "candidate at " << pattern << " " 
				<< sequenceIndex << " rock " << rockIndex << "\t" 
				<< cycle 
				<< " height " << height 
				<< " currentheight:  "<< currentHeight 
				<< "\n";
		}
		chamberPattern[key] = height;
	}
}

bool placeRockAt(const Rock& rock, __int64 rockPos, Chamber& chamber)
{
	bool filled = false;
	for (__int64 i = rock.rows.size() - 1; i >= 0; --i)
	{
		const rowType& rockRow = rock.rows[i];
		filled|=mergeRow(chamber, rockRow, rockPos);
		rockPos++;
	}
	return filled;
}

/* testleft shift row right
1111000|
0111111| & //|.......# >> #|......
-------
0111000| != 1111000|
*/
bool canMoveLeft(const Rock& rock, __int64 rockPos, const Chamber& chamber)
{
	for (__int64 i = rock.rows.size() - 1; i >= 0; --i)
	{
		rowType rockRow = (rock.rows[i] | 0x01); // +wand|
		rowType chamberRow = (chamber[rockPos] >> 1) | 0x01;
		if ( (rockRow&chamberRow) != rockRow)
		{
			return false;
		}
		rockPos++;
	}
	return true;
}

/* testright shift row left
0001111 0
1111110 0 & //#.......| << ......|#
0001110 0 != 00011110
*/
bool canMoveRight(const Rock& rock, __int64 rockPos, const Chamber& chamber)
{
	for (__int64 i = rock.rows.size() - 1; i >= 0; --i)
	{
		rowType rockRow = ((rock.rows[i] | 0x01) -1); // +wand=0|
		rowType chamberRow = ((chamber[rockPos] | 0x01) -1) << 1;
		if ((rockRow&chamberRow) != rockRow)
		{
			return false;
		}
		rockPos++;
	}
	return true;
}

/* hittest bottom
0011110|
1111011| & //inv row ....#..
--------
0011010| != 0011110|

0001000|
0011110| & //inv row ##....#|
--------
0001000| == 0001000|
*/
bool canMoveDown(const Rock& rock, __int64 rockPos, const Chamber& chamber)
{
	for (__int64 i = rock.rows.size() - 1; i >= 0; --i)
	{
		rowType rockRow = (rock.rows[i] | 0x01); // +wand|
		rowType chamberRow = (chamber[rockPos-1]) | 0x01;
		if( (rockRow&chamberRow) != rockRow)
		{ 
			return false;
		}
		rockPos++;
	}
	return true;
}

void moveLeft(Rock& rock)
{
	for (__int64 i = rock.rows.size() - 1; i >= 0; --i)
	{
		rowType& rockRow = (rock.rows[i]);
		rockRow <<= 1;
	}
}

void moveRight(Rock& rock)
{
	for (__int64 i = rock.rows.size() - 1; i >= 0; --i)
	{
		rowType& rockRow = (rock.rows[i]);
		rockRow >>= 1;
	}
}



void aoc2022_17()
{
	std::array<Rock, 5> rocks;
	initRocks(rocks);
	
	Chamber chamber;

	mergeRow(chamber, floorRow, 0);

	__int64 rockPos = top(chamber) + 4LL;
	adjustChamberHeight(chamber, rockPos+4LL);

	fs::path input(DataDir() / "2022_17.txt");
	std::ifstream inFile(input);
	constexpr auto sz = 2 << 12;
	std::string jets; jets.reserve(sz);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0) { break; }
		jets.append(line);
	}

	__int64 placedRocks = 0LL;
	__int64 deleted = 0LL;
	Rock rock = nextRock(rocks);
	__int64 cycle = 0;
	while (true)
	{
		auto [jet,sequenceIndex] = nextJet(jets);
		if (jet == '>')
		{
			if (canMoveRight(rock, rockPos, chamber))
			{
				moveRight(rock);
			}
		}
		else if (jet == '<')
		{
			if (canMoveLeft(rock, rockPos, chamber))
			{
				moveLeft(rock);
			}
		}
		if (!canMoveDown(rock, rockPos, chamber))
		{
			bool fillsAllGaps=placeRockAt(rock, rockPos, chamber);
			if (fillsAllGaps)
			{
				deleted += recalcChamber(chamber, cycle, rock.rockIndex, sequenceIndex, top(chamber) + deleted);
				//printChamber(chamber);
			}
			
			placedRocks++;
			constexpr __int64 targetCountr = 1'000'000'000'000;
			if (placedRocks % 100'000'000 == 0)
			{
				std::cout << placedRocks << " of " << targetCountr << " to go " << targetCountr - placedRocks << "\n";
			}
			if (placedRocks == 1'000'000'000'000)
			{
				//test: 3068
				//result1: 3209
				__int64 height = top(chamber)+deleted; //+1?
				std::cout << "height = " << height << "\n";//3068
				return;
			}
			rock = nextRock(rocks);


			rockPos = top(chamber) + 4LL;
			adjustChamberHeight(chamber, rockPos + 4LL);
		}
		else
		{
			--rockPos;
		}
		++cycle;
	}
	__int64 score = 0;
	std::cout << score << "\n";
}