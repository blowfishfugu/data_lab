#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>

void aoc2023_04()
{
	fs::path input(DataDir() / "2023_04.txt");
	TxtFile txt{ input };

	struct Card {
		std::string cardName{};
		__int64 cardCount{ 1 };
		__int64 bonus{};
	};

	__int64 count{};
	__int64 sum{};
	std::vector<Card> cards;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		++count;
		auto [card, pairing] = pairSplit<':', true>(line);
		auto [winning, mine] = pairSplit<'|', true>(pairing);

		std::vector<__int64> vWin = intSplit<' ', __int64>(winning);
		std::vector<__int64> vMine = intSplit<' ', __int64>(mine);
		//alternative: adjacentfind?
		std::sort(vWin.begin(), vWin.end());
		std::sort(vMine.begin(), vMine.end());
		std::vector<__int64> common;
		std::set_intersection(vWin.begin(), vWin.end(),
			vMine.begin(), vMine.end(),
			std::back_inserter(common));

		size_t cnt = common.size();
		if (cnt > 1)
		{
			cnt = 1LL << (cnt - 1);
		}
		//std::cout << std::format("{} : {:>8} : {:>4}", card, common.size(), cnt) << "\n";
		sum += cnt;
		Card newCard;
		newCard.cardName = card;
		newCard.bonus = common.size();
		cards.emplace_back(newCard);
	}
	std::cout << "count: " << count << "\n";
	std::cout << sum << "\n";
	assert(sum == 28538 || sum == 13);
	
	//"Process all of the original and copied scratchcards until no more scratchcards are won"
	__int64 sum2{};
	for (size_t i = 0; i < cards.size(); ++i)
	{
		const Card& current = cards[i];
		sum2 += current.cardCount;
		for (__int64 x = 1; x <= current.bonus; ++x) //bonus==0 -> continue
		{
			Card& next = cards[i + x];
			next.cardCount += current.cardCount;
		}
	}

	std::cout << sum2 << "\n";
	assert(sum2 == 30 || sum2 == 9425061);
}



