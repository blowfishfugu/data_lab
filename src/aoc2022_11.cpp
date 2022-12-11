#include "DataDir.h"
#include <iostream>
#include <array>
#include <deque>
#include <functional>
#include <algorithm>


auto mul = [](__int64 item, __int64 rhs)
{
	return item * rhs;
};

auto square = [](__int64 item, __int64 rhs)
{
	return item * item;
};

auto add = [](__int64 item, __int64 rhs)
{
	return item + rhs;
};

auto noop = [](__int64 item, __int64 rhs)
{
	return item;
};

struct Item
{
	__int64 initVal;
	__int64 currentVal;
	__int64 initMonkey=-1LL;
	Item(__int64 val)
		:
		initVal{val},
		currentVal{val}
	{}
};

struct Monkey
{
	size_t id = 0;
	std::deque<Item> items;
	__int64 rhs = 0LL;
	std::function<__int64(__int64,__int64)> Operation = noop;
	__int64 Test = 1LL;
	size_t Iftrue = id;
	size_t Iffalse = id;
	
	__int64 inspectionCount = 0LL;
	void initItems()
	{
		for (Item& item : items)
		{
			item.initMonkey = id;
		}
	}

	void inspect(std::vector<Monkey>& others)
	{
		/* google: substitutionsregeln fuer modulo
		( a + b) % c = ( ( a % c ) + ( b % c ) ) % c
		( a * b) % c = ( ( a % c ) * ( b % c ) ) % c
		*/

		//testmonkeys
		//constexpr __int64 alldivs = 23 * 19 * 13 * 17; //<-geraten

		//real-monkeys ->Teiler aller Tests
		//(inkl der 23 der testmonkeys, ergibt gleiches Ergebnis)
		constexpr __int64 alldivs = 17*7*3*11*19*13*2*5*23;
		while (items.size() > 0)
		{
			Item item = items.front();
			//std::cout << "M" << id <<"\t" << item.currentVal <<"\n";
			items.pop_front();
			item.currentVal = Operation(item.currentVal, rhs);
			//item /= 3;
			item.currentVal %= alldivs;
			++inspectionCount;

			if (item.currentVal%Test == 0)
			{
				others[Iftrue].items.push_back(item);
			}
			else
			{
				others[Iffalse].items.push_back(item);
			}
		}
	}
	
};

void buildTestMonkeys(std::vector<Monkey>& monkeys)
{
	monkeys.resize(4);
	// ((x*19) %23) 
	//			(23%23)=0 -> M2 (x'*x') % 13
	//			(1..22%23)=22..1-> M3 (x'+3) % 17
	Monkey& m1 = monkeys[0]; m1.id = 0;
	m1.items = { 79,98 };
	m1.rhs = 19LL;
	m1.Operation = mul;
	m1.Test = 23;
	m1.Iftrue = 2;
	m1.Iffalse = 3;

	Monkey& m2 = monkeys[1]; m2.id = 1;
	m2.items = { 54 , 65, 75, 74 };
	m2.rhs = 6LL;
	m2.Operation = add;
	m2.Test = 19;
	m2.Iftrue = 2;
	m2.Iffalse = 0;

	Monkey& m3 = monkeys[2]; m3.id = 2;
	m3.items = { 79 ,60,97 };
	m3.rhs = 0LL;
	m3.Operation = square;
	m3.Test = 13;
	m3.Iftrue = 1;
	m3.Iffalse = 3;

	Monkey& m4 = monkeys[3]; m4.id = 3;
	m4.items = { 74 };
	m4.rhs = 3LL;
	m4.Operation = add;
	m4.Test = 17;
	m4.Iftrue = 0;
	m4.Iffalse = 1;

}

void buildRealMonkeys(std::vector<Monkey>& monkeys) {
	monkeys.resize(8);

	Monkey& m0 = monkeys[0]; m0.id = 0;
	m0.items = { 78, 53, 89, 51, 52, 59, 58, 85 };
	m0.Operation = mul; m0.rhs = 3;
	m0.Test = 5;
	m0.Iftrue = 2;
	m0.Iffalse = 7;

	Monkey &m1 = monkeys[1]; m1.id = 1;
	m1.items = { 64 };
	m1.Operation = add; m1.rhs = 7;
	m1.Test = 2;
	m1.Iftrue = 3;
	m1.Iffalse = 6;

	Monkey &m2 = monkeys[2]; m2.id = 2;
	m2.items = { 71,93,65,82 };
	m2.Operation = add; m2.rhs = 5;
	m2.Test = 13;
	m2.Iftrue = 5;
	m2.Iffalse = 4;

	Monkey &m3 = monkeys[3]; m3.id = 3;
	m3.items = { 67,73,95,75,56,74 };
	m3.Operation = add; m3.rhs = 8;
	m3.Test = 19;
	m3.Iftrue = 6;
	m3.Iffalse = 0;

	Monkey& m4 = monkeys[4]; m4.id = 4;
	m4.items = { 85,91,90 };
	m4.Operation = add; m4.rhs = 4;
	m4.Test = 11;
	m4.Iftrue = 3;
	m4.Iffalse = 1;

	Monkey& m5 = monkeys[5]; m5.id = 5;
	m5.items = { 67,96,69,55,70,83,62 };
	m5.Operation = mul; m5.rhs = 2;
	m5.Test = 3;
	m5.Iftrue = 4;
	m5.Iffalse = 1;

	Monkey& m6 = monkeys[6]; m6.id = 6;
	m6.items = { 53,86,98,70,64 };
	m6.Operation = add; m6.rhs = 6;
	m6.Test = 7;
	m6.Iftrue = 7;
	m6.Iffalse = 0;

	Monkey& m7 = monkeys[7]; m7.id = 7;
	m7.items = { 88,64 };
	m7.Operation = square;// old*old;
	m7.Test = 17;
	m7.Iftrue = 2;
	m7.Iffalse = 5;
}

void aoc2022_11()
{
	fs::path input(DataDir() / "2022_11.txt");
	std::ifstream inFile(input);
	__int64 score = 0;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0 ) { break; }
	}
	std::vector<Monkey> monkeys;
	buildRealMonkeys(monkeys);
	
	for (Monkey& m : monkeys)
	{
		m.initItems();
	}

	__int64 round = 1;
	while (round <= 10000)
	{
		for (Monkey& m : monkeys)
		{
			m.inspect(monkeys);
		}
#ifndef NDEBUG
		if (round % 1000 == 0) {
		std::cout << "Round "<< round << "\n";
			for (Monkey& m : monkeys)
			{
				std::cout << m.inspectionCount << "\n";
			}
			
		}
#endif
		round++;
	}

	std::sort(monkeys.begin(), monkeys.end(), [](const Monkey& l, const Monkey& r) {
		return l.inspectionCount > r.inspectionCount;
		});
	for (Monkey& m : monkeys)
	{
		std::cout << m.inspectionCount << "\n";
	}
	score = monkeys[0].inspectionCount * monkeys[1].inspectionCount;
	std::cout << score << "\n";
}