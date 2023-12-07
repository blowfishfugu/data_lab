#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <array>
#include <map>

namespace D07 {

	static std::map<char, int> CardValues{
		{'-',0},
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

	struct Card
	{
		Card() : value{ &CardValues['-'] } {}
		explicit Card(char init) : Label{ init }, value{ &CardValues[init] }
		{
		}

		char Label{};
		int* value;
	};



	struct Hand
	{
		using Set = std::tuple<Card, Card, Card, Card, Card>;
		Set cards{};
		int type{};

		template<bool withJ>
		void evalType()
		{
			std::array<int, 15> hist{};
			hist[*std::get<0>(cards).value]++;
			hist[*std::get<1>(cards).value]++;
			hist[*std::get<2>(cards).value]++;
			hist[*std::get<3>(cards).value]++;
			hist[*std::get<4>(cards).value]++;

			int jcount = 0;

			if constexpr (withJ)
			{
				jcount = hist[1];
				if (hist[1] == 5)
				{
					type = 7;
					return;
				}
				else
				{
					hist[1] = 0;
				}
			}

			std::sort(hist.begin(), hist.end(), [](int l, int r) { return l > r; });
			int h1 = hist[0];
			h1 += jcount;

			int h2 = hist[1];
			if (h1 == 5)
			{
				type = 7;
			}
			else if (h1 == 4)
			{
				type = 6;
			}
			else if (h1 == 3 && h2 == 2)
			{
				type = 5;
			}
			else if (h1 == 3)
			{
				type = 4;
			}
			else if (h1 == 2 && h2 == 2)
			{
				type = 3;
			}
			else if (h1 == 2)
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
				std::format("{}{}{}{}{}\t{}\t{}\n",
				std::get<0>(cards).Label,
				std::get<1>(cards).Label,
				std::get<2>(cards).Label,
				std::get<3>(cards).Label,
				std::get<4>(cards).Label,
				type,
				bid
				) };
			std::cout << str;
		}
	};

	auto operator<=>(const Card& l, const Card& r)
	{
		return *l.value <=> *r.value;
	}

	auto operator<=>(const Hand& l, const Hand& r)
	{
		std::weak_ordering byType = (l.type <=> r.type);
		if (byType != 0)
		{
			return byType;
		}
		return (l.cards <=> r.cards);
	}

}

void aoc2023_07()
{
	using namespace D07;
	fs::path input(DataDir() / "2023_07.txt");
	TxtFile txt{ input };


	std::vector<Hand> hands;

	__int64 count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		auto [cardstring, bid] = pairSplit<' ', true>(line);
		Hand h;
		h.bid = toInt<__int64>(bid);

		int i = 0;
		for (char c : cardstring)
		{
			Card card(c);
			switch (i)
			{
			case(0): std::get<0>(h.cards) = card; break;
			case(1): std::get<1>(h.cards) = card; break;
			case(2): std::get<2>(h.cards) = card; break;
			case(3): std::get<3>(h.cards) = card; break;
			case(4): std::get<4>(h.cards) = card; break;
			default:
				break;
			}
			++i;
		}
		hands.emplace_back(h);
		++count;
	}
	std::cout << "count: " << count << "\n";

	for (Hand& h : hands)
	{
		h.evalType<false>();
	}
	std::sort(hands.begin(), hands.end());
	__int64 rank = 1LL;
	__int64 bidsPerRank{};
	for (Hand& h : hands)
	{
		bidsPerRank += h.bid * rank;
		++rank;
	}

	std::cout << "bids : " << bidsPerRank << "\n";
	assert(bidsPerRank == 6440 || bidsPerRank == 253313241);

	CardValues['J'] = 1;
	for (Hand& h : hands)
	{
		h.evalType<true>();
	}
	std::sort(hands.begin(), hands.end());

	rank = 1LL;
	bidsPerRank = 0;
	for (Hand& h : hands)
	{
		bidsPerRank += h.bid * rank;
		++rank;
	}
	std::cout << "jbids: " << bidsPerRank << "\n";
	assert(bidsPerRank == 5905 || bidsPerRank == 253362743);
	//idea for part3?: Jokers can be replaced by any available card (including new value)
	//to fulfill the highest possible ranking, so JTKTK will be KTKTK instead of TTKTK
}

