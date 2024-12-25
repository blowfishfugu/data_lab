#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <array>

void aoc2024_25()
{
	fs::path input(DataDir() / "2024_25.txt");
	TxtFile txt{ input };

	using teeth = std::array<std::int8_t, 5>;
	constexpr const teeth fullSig{ '#','#','#','#','#' };
	constexpr const teeth nullSig{ '.','.','.','.','.' };
	std::vector<teeth> Locks{};
	std::vector<teeth> Keys{};

	for (std::vector<teeth> lineset{}; const auto& line : txt)
	{
		if (line.size() == 0 || line.size() > 5) { continue; }
		teeth fromLine{};
		for (int i = 0; char c : line) { fromLine[i++] = static_cast<std::int8_t>(c); }

		if (lineset.size() == 6) {
			teeth Sig{};
			for (int y = 1; y < 6; ++y) {
				for (int x = 0; x < 5; ++x) {
					char n = lineset[y][x];
					Sig[x] += (n == '#') ? 1 : 0;
				}
			}
			if (lineset[0] == fullSig && fromLine == nullSig)
			{
				Keys.emplace_back(Sig);
			}
			else if (lineset[0] == nullSig && fromLine == fullSig)
			{
				Locks.emplace_back(Sig);
			}
			lineset.clear();
			continue;
		}
		lineset.push_back(fromLine);
	}
	
	std::int64_t fits{};
	for (const teeth& Lock : Locks) {
		for (const teeth& Key : Keys) {
			std::int8_t keyok = 1;
			for (int i = 0; i < 5; ++i) {
				if (Lock[i] + Key[i] > 5) {
					keyok = 0;
					break;
				}
			}
			fits+=keyok;
		}
	}
	std::cout << fits << " fits \n";
}

