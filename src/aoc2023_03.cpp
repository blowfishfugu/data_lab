#include "DataDir.h"
#include "LineIterator.h"
#include "SplitIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <algorithm>

void aoc2023_03()
{
	fs::path input(DataDir() / "2023_03.txt");
	TxtFile txt{ input };
	using Grid = std::vector<std::string_view>;
	Grid grid;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		grid.emplace_back(line);
	}
	std::cout << "rowcount: " << grid.size() << "\n";
	
	struct anyItem {
		__int64 col{};
		std::string_view txt{};
		std::string_view symbol{};
	};
	std::vector<std::vector<anyItem>> parts;
	std::vector<std::vector<anyItem>> symbols;
	parts.resize(grid.size());
	symbols.resize(grid.size());

	for (size_t row = 0; row < grid.size(); ++row)
	{
		std::string_view line = grid[row];
		for (__int64 c = 0; c < static_cast<__int64>(line.size());++c)
		{
			char tst = line[c];
			if (tst == '.') { continue; }

			if (tst >= '0' && tst <= '9')
			{
				size_t more = c + 1;
				while (more < line.size())
				{
					if (line[more] == '.') { break; }
					if (line[more] < '0' || line[more] > '9') { break; }
					++more;
				}
				size_t strLen = more - c;
				std::string_view txt(line.data()+c,strLen );
				anyItem part{ .col = c, .txt{txt}, .symbol{} };
				parts[row].emplace_back(part);
				c = more - 1;
			}
			else
			{
				std::string_view txt(line.data() + c, 1);
				anyItem symbol{ .col = c, .txt = {txt}, .symbol = {txt} };
				symbols[row].emplace_back(symbol);
			}
		}
	}

	auto hasSymbolInRange = [](decltype(symbols) symbols, anyItem& part, __int64 left, __int64 right, __int64 top, __int64 bottom) {
		for (__int64 symbolLine = top; symbolLine <= bottom; ++symbolLine)
		{
			for (anyItem& symbol : symbols[symbolLine])
			{
				__int64 sc = symbol.col;
				if (sc >= left && sc <= right)
				{
					part.symbol = symbol.symbol;
					return true;
				}
			}
		}
		return false;
	};

	__int64 sum{};
	__int64 rowCount = static_cast<__int64>(parts.size());
	for (__int64 row = 0; row < rowCount; ++row)
	{
		for (anyItem& part : parts[row])
		{
			__int64 left = std::max(part.col - 1LL,0LL);
			__int64 right = std::min(part.col + part.txt.size(), grid[row].size() - 1);
			__int64 top = std::max(row - 1LL, 0LL);
			__int64 bottom = std::min(row + 1LL, rowCount - 1LL);
			if (hasSymbolInRange(symbols, part, left, right, top, bottom))
			{
				__int64 partIdent{};
				std::from_chars(part.txt.data(), part.txt.data() + part.txt.size(), partIdent);
				sum += partIdent;
			}
		}
	}
	std::cout << "sum of partnumbers: " << sum << "\n";
}

