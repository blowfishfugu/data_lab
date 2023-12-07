#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <array>
#include <map>



struct Card
{
	Card(){}
	explicit Card( char init, bool specialJ ) : Label{ init }
	{
		static std::map<char, int> CardValues{
			{'2',2},
			{'3',3},
			{'4',4},
			{'5',5},
			{'6',6},
			{'7',7},
			{'8',8},
			{'9',9},
			{'T',10},
			{'J',11},
			{'Q',12},
			{'K',13},
			{'A',14},
		};

		if (specialJ) { CardValues['J'] = 1; }

		value = CardValues[ init ];
	}
	char Label{};
	int value{};
};



struct Hand
{
	using Set = std::tuple<Card, Card, Card, Card, Card>;
	Set cards{};
	int type{};
	void evalType()
	{
		std::array<int, 15> hist{};
		hist[ std::get<0>( cards ).value ]++;
		hist[ std::get<1>( cards ).value ]++;
		hist[ std::get<2>( cards ).value ]++;
		hist[ std::get<3>( cards ).value ]++;
		hist[ std::get<4>( cards ).value ]++;

		
		int jcount = hist[1];
		if (hist[1] == 5)
		{
			type = 7;
			return;
		}
		else
		{
			hist[1] = 0;
		}


		std::sort( hist.begin(), hist.end(), []( int l, int r ) { return l > r; } );
		int h1 = hist[ 0 ];
		h1 += jcount;

		int h2 = hist[ 1 ];
		if( h1 == 5 )
		{
			type = 7;
		}
		else if( h1 == 4 )
		{
			type = 6;
		}
		else if( h1 == 3 && h2 == 2 )
		{
			type = 5;
		}
		else if( h1 == 3 )
		{
			type = 4;
		}
		else if( h1 == 2 && h2 == 2 )
		{
			type = 3;
		}
		else if( h1 == 2 )
		{
			type = 2;
		}
		else
		{
			type = 1;
		}
	}
	__int64 bid{};

	void print()
	{
		std::string str{
			std::format( "{}{}{}{}{}\t{}\t{}\n",
			std::get<0>( cards ).Label,
			std::get<1>( cards ).Label,
			std::get<2>( cards ).Label,
			std::get<3>( cards ).Label,
			std::get<4>( cards ).Label,
			type,
			bid
			)};
		std::cout << str;
	}
};

auto operator<=>( const Card& l, const Card& r )
{
	return l.value <=> r.value;
}

auto operator<=>( const Hand& l, const Hand& r )
{
	std::weak_ordering byType = (l.type <=> r.type);
	if( byType != 0 )
	{
		return byType;
	}
	auto cardOrder= (l.cards <=> r.cards);
	return cardOrder;
}

void aoc2023_07()
{
	fs::path input(DataDir() / "2023_07.txt");
	TxtFile txt{ input };

	
	std::vector<Hand> hands;

	__int64 count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		auto [cardstring,bid]= pairSplit<' ', true>( line );
		Hand h;
		h.bid = toInt<__int64>( bid );

		int i = 0;
		for( char c : cardstring )
		{
			Card card( c, true );
			switch( i )
			{
				case( 0 ): std::get<0>( h.cards ) = card; break;
				case( 1 ): std::get<1>( h.cards ) = card; break;
				case( 2 ): std::get<2>( h.cards ) = card; break;
				case( 3 ): std::get<3>( h.cards ) = card; break;
				case( 4 ): std::get<4>( h.cards ) = card; break;
				default:
					break;
			}
			++i;
		}
		h.evalType();
		hands.emplace_back( h );
		++count;
	}

	std::sort( hands.begin(), hands.end() );
	__int64 rank = 1LL;
	__int64 bidsPerRank{};
	for( Hand& h : hands )
	{
		std::cout << rank << " :";
		h.print();
		bidsPerRank += h.bid * rank;
		++rank;
	}

	std::cout << "bids : " << bidsPerRank << "\n";
	std::cout << "count: " << count << "\n";
	//assert(count == 1000);
}

