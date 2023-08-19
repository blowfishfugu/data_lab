#ifdef UNIT_TEST
#include "LineIterator.h"
#include <iostream>
#include <format>
#include <algorithm>
#include <iomanip>

bool runcheck( std::string_view const& toSplit, int expectedItemCount) {
	LineIterator begin{ toSplit };
	LineIterator end{};
	int hits = 0;
	for (; begin != end; ++begin)
	{
		std::string toPrint(*begin);
		std::replace(toPrint.begin(), toPrint.end(), '\r', '&');
		std::cout << hits<<"\t"<< std::quoted(toPrint,'\'') << "\n";
		++hits;
	}
	std::cout << std::format("{} == {}?\n\n", hits, expectedItemCount);
	return hits == expectedItemCount;
}

void IteratorCheck()
{
	bool res = true;
	res &= runcheck("", 0);
	res &= runcheck("a", 1);
	res &= runcheck("a\nb\nc\n", 3);
	res &= runcheck("a\r\nb\r\nc", 3);
	res &= runcheck("a\n-\nb\r\nc", 4);
	res &= runcheck("a\n\nb\r\nc", 4);
	res &= runcheck("\n\n\r\n\n", 4);
	res &= runcheck(R"xx(res &= runcheck("", 0);
	res &= runcheck("a", 1);
	res &= runcheck("a\nb\nc\n", 3);
	res &= runcheck("a\r\nb\r\nc", 3);
	res &= runcheck("a\n-\nb\r\nc", 4);
	res &= runcheck("a\n\nb\r\nc", 4);)xx", 6);
	if (!res)
	{
		struct IteratorCheckFailed{};
		throw IteratorCheckFailed{};
	}
}

#endif