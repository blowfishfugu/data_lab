#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
namespace D02 {
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


	gems countColors(const Item& line) {
		SplitIterator<','> colBegin{ line }; SplitIterator<','> colEnd{};
		gems g;
		while (colBegin != colEnd)
		{
			auto [count, colorName] = pairSplit<' ', true>(*colBegin);
			const int iCount = toInt<int>(count);
			if (colorName == "red") { g.r = iCount; }
			else if (colorName == "green") { g.g = iCount; }
			else if (colorName == "blue") { g.b = iCount; }
			++colBegin;
		}
		return g;
	}

	void addGame(const std::string_view& line, Games& games)
	{
		auto [gameInfo, rounds] = pairSplit<':'>(line);
		auto [gamePrefix, gameId] = pairSplit<' '>(gameInfo);
		game g;
		g.id = toInt<int>(gameId);

		SplitIterator<';'> rndBegin{ rounds }; SplitIterator<';'> rndEnd{};
		while (rndBegin != rndEnd)
		{
			Item round = *rndBegin;
			gems stones = countColors(round);
			g.rounds.emplace_back(stones);
			++rndBegin;
		}
		games.emplace_back(g);
	}
}

void aoc2023_02()
{
	using namespace D02;
	fs::path input(DataDir() / "2023_02.txt");
	TxtFile txt{ input };

	Games games;
	int count{};
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		addGame(line, games);
	}
	std::cout << "count: " << games.size() << "\n";

	const gems allowed{ .r = 12, .g = 13, .b = 14 };
	for (game& g : games){
		for (const gems& round : g.rounds){
			if ( round.r > allowed.r || round.g > allowed.g	|| round.b > allowed.b){
				g.possible = false;
			}
		}
	}

	const auto firstImpossible = std::partition( games.begin(), games.end(),
		[](const game& test) {return test.possible; }
	);

	int sumOfIds{};
	for ( auto it = games.cbegin(); it != firstImpossible; ++it ){
		sumOfIds += (*it).id;
	}
	std::cout << "chkPossibles: " << sumOfIds << "\n";
	assert(sumOfIds == 2447);

	int sumOfPowers{};
	for (const game& g : games){
		if (g.rounds.size() == 0) { continue; }

		gems minGems = g.rounds[0];
		for (size_t i = 1; i < g.rounds.size(); ++i){
			const gems& current = g.rounds[i];
			minGems.r = std::max(minGems.r, current.r);
			minGems.g = std::max(minGems.g, current.g);
			minGems.b = std::max(minGems.b, current.b);
		}
		sumOfPowers += (minGems.r * minGems.g * minGems.b);
	}
	std::cout << "sumOfPowers: " << sumOfPowers << "\n";
	assert(sumOfPowers == 56322);
}

