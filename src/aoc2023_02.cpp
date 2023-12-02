#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "SplitIterator.h"
#include <iostream>
#include <cassert>

struct gems {
	__int64 r{};
	__int64 g{};
	__int64 b{};
};

struct game {
	__int64 id{};
	bool possible{ true };
	std::vector<gems> rounds{};
};

using Games = std::vector<game>;
using Item = std::string_view;

template<std::integral Int>
Int toInt(const Item& item)
{
	Int num{};
	std::from_chars(item.data(), item.data() + item.size(), num);
	//TODO: ErrorCheck? or throw?
	return num;
}

template<char c>
std::tuple<Item,Item> pairSplit( const std::string_view& line)
{
	Item trimmed = line;
	while (trimmed[0] == ' ') {	trimmed.remove_prefix(1); }
	while (trimmed[trimmed.size()-1] == ' ') {	trimmed.remove_suffix(1); }
	
	SplitIterator<c> it{ trimmed }; SplitIterator<c> itEnd{};
	if (it != itEnd) {
		Item first{ *it };
		++it;
		if (it != itEnd) {
			Item second{ *it };
			return { first,second };
		}
		else { 
			return { first,"" }; 
		}
	}
	return {};
}

gems countColors(const Item& line) {
	SplitIterator<','> colBegin{ line }; SplitIterator<','> colEnd{};
	gems g;
	while (colBegin != colEnd)
	{
		auto [count, colorName] = pairSplit<' '>(*colBegin);
		const __int64 iCount = toInt<__int64>(count);
		if (colorName == "red") { g.r = iCount; }
		else if (colorName == "green") { g.g = iCount; }
		else if (colorName == "blue") { g.b = iCount; }
		++colBegin;
	}
	return g;
}

void addGame(const std::string_view line, Games& games)
{
	auto [gameInfo, rounds] = pairSplit<':'>(line);
	auto [gamePrefix, gameId] = pairSplit<' '>(gameInfo);
	game g;
	g.id = toInt<__int64>(gameId);
	
	SplitIterator<';'> rndBegin{ rounds }; SplitIterator<';'> rndEnd{};
	while (rndBegin != rndEnd)
	{
		Item round = *rndBegin;
		gems stones=countColors(round);
		g.rounds.emplace_back(stones);
		++rndBegin;
	}
	
	games.emplace_back(g);
}

void aoc2023_02()
{
	fs::path input(DataDir() / "2023_02.txt");
	TxtFile txt{ input };

	Games games;
	__int64 count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		addGame(line, games);
	}
	std::cout << "count: " << games.size() << "\n";

	const gems allowed{ .r = 12, .g = 13, .b = 14 };
	for (game& g : games)
	{
		for (const gems& round : g.rounds)
		{
			if (round.r > allowed.r) { g.possible = false; }
			if (round.g > allowed.g) { g.possible = false; }
			if (round.b > allowed.b) { g.possible = false; }
			//if(!g.possible){ break; } ??
		}
	}

	auto firstImpossible=std::partition(
		games.begin(), games.end(),
		[](const game& test) {return test.possible; }
	);

	__int64 sumOfIds{};
	for (auto it=games.begin(); it!=firstImpossible; ++it)
	{
		sumOfIds += (*it).id;
	}
	std::cout << "chkPossibles: " << sumOfIds << "\n";
	assert(sumOfIds == 2447);

	__int64 sumOfPowers{};
	for (game& g : games)
	{
		gems minGems = g.rounds[0];
		for (size_t i=1;i<g.rounds.size();++i)
		{
			const gems& current = g.rounds[i];
			minGems.r = std::max(minGems.r, current.r);
			minGems.g = std::max(minGems.g, current.g);
			minGems.b = std::max(minGems.b, current.b);
		}
		sumOfPowers += (minGems.r*minGems.g*minGems.b);
	}
	std::cout << "sumOfPowers: " << sumOfPowers << "\n";
	assert(sumOfPowers == 56322);
}

