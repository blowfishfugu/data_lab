#include "DataDir.h"
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <map>
#include <tuple>

struct SDigit
{
	char c = '0';
	__int64 order = 0;
	int val = 0;
	SDigit(): order(0), c('0'), val(0)
	{}

	SDigit(__int64 order, char in)
		:order(order),c(in)
	{
		charToVal();
	}

	void charToVal()
	{
		switch (c)
		{
		case('='): val = -2; break;
		case('-'): val = -1; break;
		case('0'): val = 0; break;
		case('1'): val = 1; break;
		case('2'): val = 2; break;
		default://leerzeichen, tabs usw..
			c = '\0'; val = 0;
			break; //ignore
		}
	}
};

struct SNumber
{
	std::vector<SDigit> digits;
	SNumber(std::string_view rep)
	{
		const __int64 len = rep.length();
		if (len > 0)
		{
			for (__int64 i = len; i > 0; --i)
			{
				SDigit d{ len-i,rep[i-1LL] };
				if (d.c != '\0')
				{
					digits.emplace_back(d);
				}
			}
		}
	}

	SNumber(__int64 carryLen)
	{
		if (carryLen > 0)
		{
			for (__int64 i = carryLen; i > 0; --i)
			{
				SDigit d{ carryLen-i,'0' };
				digits.emplace_back(d);
			}
		}
	}

	void trimLeading()
	{
		while (digits.size() > 0)
		{
			const SDigit& d = digits[digits.size() - 1];
			if (d.c != '0') { break; }
			
			digits.pop_back();
		}
	}

	void print() const
	{
		std::string val;
		constexpr __int64 cap = 20; //max numberlen aus eingabedaten
		while ( val.size()+digits.size() < cap)
		{
			val.push_back(' ');
		}
		for (auto it = digits.rbegin(); it != digits.rend(); it++)
		{
			const SDigit d = *it;
			val.push_back(d.c);
		}
		std::cout << val << "\n";
	}
};

//lhs,rhs
using Addition = std::tuple<char, char>;
//Carry, NewDigit
using Result = std::pair<SDigit, SDigit>;

static const std::map<const Addition, Result> addRules={
		{ {'=','='},  { {1,'-'},{0,'1'} } },
		{ {'=','-'},  { {1,'-'},{0,'2'} } },
		{ {'=','0'},  { {1,'0'},{0,'='} } },
		{ {'=','1'},  { {1,'0'},{0,'-'} } },
		{ {'=','2'},  { {1,'0'},{0,'0'} } },
		
		{ {'-','='},  { {1,'-'},{0,'2'} } },
		{ {'-','-'},  { {1,'0'},{0,'='} } },
		{ {'-','0'},  { {1,'0'},{0,'-'} } },
		{ {'-','1'},  { {1,'0'},{0,'0'} } },
		{ {'-','2'},  { {1,'0'},{0,'1'} } },

		{ {'0','='},  { {1,'0'},{0,'='} } },
		{ {'0','-'},  { {1,'0'},{0,'-'} } },
		{ {'0','0'},  { {1,'0'},{0,'0'} } },
		{ {'0','1'},  { {1,'0'},{0,'1'} } },
		{ {'0','2'},  { {1,'0'},{0,'2'} } },

		{ {'1','='},  { {1,'0'},{0,'-'} } },
		{ {'1','-'},  { {1,'0'},{0,'0'} } },
		{ {'1','0'},  { {1,'0'},{0,'1'} } },
		{ {'1','1'},  { {1,'0'},{0,'2'} } },
		{ {'1','2'},  { {1,'1'},{0,'='} } },

		{ {'2','='},  { {1,'0'},{0,'0'} } },
		{ {'2','-'},  { {1,'0'},{0,'1'} } },
		{ {'2','0'},  { {1,'0'},{0,'2'} } },
		{ {'2','1'},  { {1,'1'},{0,'='} } },
		{ {'2','2'},  { {1,'1'},{0,'-'} } },
	};

SNumber Add(const SNumber& lhs, const SNumber& rhs)
{
	__int64 carryLen = std::max(lhs.digits.size(), rhs.digits.size());
	SNumber carries(1);
	SNumber result(0);
	bool carryBit = false;
	for (__int64 order = 0; order < carryLen; order++)
	{
		char left = '0';
		if (order < lhs.digits.size()) { left = lhs.digits[order].c; }
		char right = '0'; 
		if (order < rhs.digits.size()) { right = rhs.digits[order].c; }

		const Addition key = std::tie(left, right);
		if (const auto& f = addRules.find(key); f != addRules.end())
		{
			Result added = f->second;
				if (added.first.c != '0')
				{
					carryBit = true;
				}
			carries.digits.emplace_back(added.first);
			result.digits.emplace_back(added.second);
		}
	}
	carries.trimLeading();
	result.trimLeading();

	if (carryBit)
	{
		result = Add(result, carries);
	}

	return result;
}

void testAdd(std::string_view lhs, std::string_view rhs)
{
	std::cout << "\n\n";
	SNumber one(lhs);
	SNumber res(rhs);
	one.print();
	res.print();
	res = Add(res, one);
	res.print();
	std::cout << "\n\n";
}

void aoc2022_25()
{
	std::vector<SNumber> numbers;
	fs::path input(DataDir() / "2022_25.txt");
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0) { break; }
		SNumber number(line);
		//number.print();
		numbers.emplace_back(number);
	}

	//testAdd("1", "2");
	//testAdd("12", "1");
	//testAdd("2", "1=");
	//testAdd("1=", "1=");
	//testAdd("1=", "1-");
	//testAdd("2=", "2=");
	//testAdd("2=", "12");
	//testAdd("10", "20");
	
	

	SNumber sum("0");
	for (__int64 i = 0; i < numbers.size(); i++)
	{
		sum = Add(sum, numbers[i]);
	}
	sum.print(); // 2=--=0000-1-0-=1=0=2
	
}