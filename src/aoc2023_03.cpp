#include "DataDir.h"
#include "LineIterator.h"
#include "SplitIterator.h"
#include "InputConversions.h"
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
	std::cout << "count: " << grid.size() << "\n";
	
	struct PartNumber {
		__int64 col{};
		__int64 len{};
		__int64 value{};
		std::vector<char> symbols{};
	};
	struct Symbol {
		__int64 col{};
		char txt;
		std::vector<__int64> connected{};
	};

	std::vector<std::vector<PartNumber>> parts;
	std::vector<std::vector<Symbol>> symbols;
	parts.resize(grid.size());
	symbols.resize(grid.size());

	auto consumeNumber = [](__int64 c, std::string_view line, std::vector<PartNumber>& parts) {
		size_t more = c + 1;
		while (more < line.size())
		{
			if (line[more] == '.') { break; }
			if (line[more] < '0' || line[more] > '9') { break; }
			++more;
		}
		__int64 strLen = more - c;
		std::string_view txt(line.data() + c, strLen);
		__int64 partIdent = toInt<__int64>(txt);
		
		PartNumber part{ .col = c, .len = strLen, .value = partIdent, .symbols={} };
		parts.emplace_back(part);
		c = more - 1;
		return c;
	};
	auto consumeSymbol = [](__int64 c, char letter, std::vector<Symbol>& symbols) {
		Symbol symbol{ .col = c, .txt = letter, .connected = {} };
		symbols.emplace_back(symbol);
	};

	for (size_t row = 0; row < grid.size(); ++row)
	{
		std::string_view line = grid[row];
		for (__int64 c = 0; c < static_cast<__int64>(line.size());++c)
		{
			char tst = line[c];
			if (tst == '.') { continue; }

			if (tst >= '0' && tst <= '9')
			{
				c = consumeNumber(c, line, parts[row]);
			}
			else
			{
				consumeSymbol(c, tst, symbols[row]);
			}
		}
	}

	auto connectSymbolsInRange = [](decltype(symbols)& symbols, PartNumber& part,
		__int64 left, __int64 right, __int64 top, __int64 bottom
		) 
	{
			for (__int64 symbolLine = top; symbolLine <= bottom; ++symbolLine)
			{
				for (Symbol& symbol : symbols[symbolLine])
				{
					__int64 sc = symbol.col;
					if (sc >= left && sc <= right)
					{
						part.symbols.emplace_back(symbol.txt);
						if (symbol.txt == '*') {
							symbol.connected.push_back(part.value);
						}
					}
				}
			}
			return part.symbols.size();
	};

	const __int64 maxRow = static_cast<__int64>(parts.size())-1LL;
	const __int64 maxCol = static_cast<__int64>(grid[0].size())-1LL;

	__int64 sum{};
	for (__int64 row = 0; row <= maxRow; ++row)
	{
		for (PartNumber& part : parts[row])
		{
			//BB?
			__int64 left = std::max(part.col - 1LL, 0LL);
			__int64 right = std::min(part.col + part.len, maxCol);
			__int64 top = std::max(row - 1LL, 0LL);
			__int64 bottom = std::min(row + 1LL, maxRow);
			if (connectSymbolsInRange(symbols, part, left, right, top, bottom) > 0LL)
			{
				sum += part.value;
			}
		}
	}
	std::cout << "sum of partnumbers: " << sum << "\n";
	assert(sum == 528819 || sum == 4361);

	__int64 sumOfRatios{};
	for (const auto& symbolLine : symbols)
	{
		for (const auto& symbol : symbolLine)
		{
			if (symbol.connected.size() == 2)
			{
				const __int64& i1 = symbol.connected[0];
				const __int64& i2 = symbol.connected[1];
				const __int64 ratio = i1 * i2;
				sumOfRatios += ratio;
			}
		}
	}
	std::cout << "sum of ratios: " << sumOfRatios << "\n";
	assert(sumOfRatios == 80403602 || sumOfRatios == 467835);
}

