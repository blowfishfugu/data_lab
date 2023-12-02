#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "SplitIterator.h"
#include <iostream>
#include <cassert>

struct gems {
	int r{};
	int g{};
	int b{};
};

struct game {
	int id{};
	bool possible{ true };
	std::vector<gems> rounds{};
};

using Games = std::vector<game>;
using Item = std::string_view;

int toInt(const std::string_view& item)
{
	int num{};
	std::from_chars(item.data(), item.data() + item.size(), num);
	//TODO: ErrorCheck? or throw?
	return num;
}

template<char c>
std::tuple<Item,Item> pairSplit( const std::string_view& line)
{
	std::string_view trimmed = line;
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
		int iCount = toInt(count);
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
	g.id = toInt(gameId);
	
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

	gems allowed{ .r = 12, .g = 13, .b = 14 };
	for (game& g : games)
	{
		for (const gems& round : g.rounds)
		{
			if (round.r > allowed.r) { g.possible = false; }
			if (round.g > allowed.g) { g.possible = false; }
			if (round.b > allowed.b) { g.possible = false; }
		}
	}

	auto firstImpossible=std::partition(games.begin(), games.end(), [](const game& test) {return test.possible; });
	__int64 sumOfIds{};
	for (auto it=games.begin(); it!=firstImpossible; ++it)
	{
		sumOfIds += (*it).id;
	}
	std::cout << "chkPossibles: " << sumOfIds << "\n";
	//assert(count == 42);
}

