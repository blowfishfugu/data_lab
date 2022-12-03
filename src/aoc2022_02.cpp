#include "DataDir.h"
#include <iostream>
#include <array>

void aoc2022_02()
{
	fs::path input(DataDir() / "2022_02.txt");

	std::ifstream inFile(input);
	
	struct Sign
	{
		char Type=0;
		Sign* whats_better = nullptr;
		Sign* draw = nullptr;
		Sign* whats_worse = nullptr;
		int value = 0;
		Sign* xyz[3]={0,0,0};
		void mapResults()
		{
			xyz[0] = whats_worse; //X
			xyz[1] = draw; //Y
			xyz[2] = whats_better; //Z
		}
	};

	std::array<Sign, 3> abc;
	auto getSource = [&abc](char symbol)->Sign*{
		if (symbol >= 'A' && symbol <= 'C')
		{
			return &abc[(size_t)symbol - 'A'];
		}
		return nullptr;
	};
	auto getDst = [&abc](char symbol)->Sign*{ 
		if (symbol >= 'X' && symbol <= 'Z')
		{
			return &abc[(size_t)symbol - 'X'];
		}
		return nullptr;
	};
	auto getDst2 = [](char lose_draw_win, Sign* source)->Sign* {
		if (lose_draw_win >= 'X' && lose_draw_win <= 'Z')
		{
			return source->xyz[(size_t)lose_draw_win - 'X'];
		}
		return nullptr;
	};
	auto calcScore = [](Sign* other, Sign* mine)->__int64 {
		if (other == mine)
		{
			return (mine->value + 3LL);
		}
		else if (other->whats_better == mine)
		{
			return (mine->value + 6LL);
		}
		return mine->value + 0LL;
	};

	Sign* Rock = getSource('A'); Rock->draw = Rock;
	Sign* Paper = getSource('B'); Paper->draw = Paper;
	Sign* Scissor = getSource('C'); Scissor->draw = Scissor;
	
	Rock->Type = 'R'; 
	Rock->whats_better = Paper;
	Rock->whats_worse = Scissor;
	Rock->value = 1;
	
	Paper->Type = 'P';
	Paper->whats_better = Scissor;
	Paper->whats_worse = Rock; 
	Paper->value = 2;
	
	Scissor->Type = 'S';
	Scissor->whats_better = Rock;
	Scissor->whats_worse = Paper;
	Scissor->value = 3;

	for (Sign& s : abc) { s.mapResults(); }

	__int64 score = 0;
	__int64 bait = 0;
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() < 3) { continue; }
		char from = line[0];
		char to = line[2];
		Sign* source = getSource(from);
		Sign* dst = getDst(to);
		Sign* dst2 = getDst2(to,source);
		if (!source || !dst || !dst2) { continue; }

		score += calcScore(source, dst);
		bait += calcScore(source, dst2);
	}
	std::cout << score << "\n";
	std::cout << bait << "\n";
}