#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"

#include <iostream>
#include <format>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>

void aoc2015_06()
{
	TxtFile txt{ DataDir() / "2015_06.txt" };

	for (const auto& line : txt)
	{
	}
}