#include "DataDir.h"
#include <iostream>
#include <array>
#include <vector>
#include <initializer_list>
#include <string>

bool allSet(std::initializer_list<std::string> lst)
{
	for (const auto& str : lst)
	{
		if (str.length() == 0) { return false; }
	}
	return true;
}

struct range {
	int l;
	int r;
	int len;
	range(int _l, int _r)
		:l(_l), r(_r),
		len(_r - _l + 1)
	{

	}

	range(std::string& sl, std::string& sr)
		:range{ std::stoi(sl), std::stoi(sr) }
	{
	}
};

range intersect(const range& lhs, const range& rhs)
{
	return range{ rhs.l - lhs.l, rhs.r - lhs.r };
}

void aoc2022_04()
{
	fs::path input(DataDir() / "2022_04.txt");
	std::ifstream inFile(input);
	std::vector<std::tuple<range, range, range>> lines;
	__int64 fullyContained = 0;
	while (!inFile.eof())
	{
		std::string s1, s2, s3, s4;
		std::getline(inFile, s1, '-');
		if (s1[0] == '\n')
		{
			break;
		}
		std::getline(inFile, s2, ',');
		std::getline(inFile, s3, '-');
		std::getline(inFile, s4);
		if (!allSet({ s1,s2,s3,s4 }))
		{
			break;
		}
		lines.emplace_back(range{ s1,s2 }, range{ s3,s4 }, range{ 0,0 });
	}

	__int64 hit = 0;
	for (auto& x : lines)
	{
		range& l = std::get<0>(x);
		range& r = std::get<1>(x);
		range& diff = std::get<2>(x);
		diff = intersect(l, r);
		if (diff.l >= 0 && diff.r <= 0)
		{
			fullyContained++;
		}
		else
		{
			diff = intersect(r, l);
			if (diff.l >= 0 && diff.r <= 0)
			{
				fullyContained++;
			}
		}

		if (l.l >= r.l && l.l <= r.r)
		{
			hit++;
		}
		else if (l.r >= r.l && l.r <= r.r)
		{
			hit++;
		}
		else
		{
			std::swap(l, r);
			if (l.l >= r.l && l.l <= r.r)
			{
				hit++;
			}
			else if (l.r >= r.l && l.r <= r.r)
			{
				hit++;
			}
		}


	}

	std::cout << "full " << fullyContained << "\n";
	std::cout << "any " << hit << "\n";
}