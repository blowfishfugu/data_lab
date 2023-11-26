#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>

void aoc2023_25()
{
	fs::path input(DataDir() / "2023_25.txt");
	TxtFile txt{ input };

	__int64 count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { continue; }
		++count;
	}
	std::cout << "count: " << count << "\n";
	assert(count == 42);
}

