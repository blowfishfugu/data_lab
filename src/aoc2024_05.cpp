#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>

void aoc2024_05()
{
	fs::path input(DataDir() / "2024_05.txt");
	TxtFile txt{ input };

	__int64 count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		++count;
	}
	std::cout << "count: " << count << "\n";
	assert(count == 42);
}

