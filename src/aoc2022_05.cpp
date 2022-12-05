#include "DataDir.h"
#include <iostream>
#include <array>
#include <memory>
#include <vector>
#include <sstream>

struct Crate
{
	char symbol = '\0';
	Crate* below = nullptr;
	Crate(char name)
		: symbol(name)
	{}
};
struct Cargo
{
	std::vector<std::unique_ptr<Crate>> pool;
	Crate* getCrate(char name) {
		pool.emplace_back(std::make_unique<Crate>(name));
		Crate* ptr = pool[pool.size() - 1].get();
		return ptr;
	}
};

struct Stack
{
	Crate* top=nullptr;
	Crate* root=nullptr;
	char id;
	Stack(char name, Cargo& pool) :id(name)
	{
		root = pool.getCrate(name);
		top = root;
	}
	void AddCrate(char name, Cargo& pool)
	{
		Crate* newTop = pool.getCrate(name);
		newTop->below = top;
		top = newTop;
	}
	void MoveTopTo(Stack& target)
	{
		if (top == root)
		{
			std::cout << "!! wants to move ground !!\n";
			return;
		}
		Crate* moved = top;
		top = top->below;
		moved->below = target.top;
		target.top = moved;
	}

	void MoveCountTo(int count, Stack& target)
	{
		if (top == root)
		{
			std::cout << "!! wants to move ground !!\n";
			return;
		}
		Crate* movedTop = top;
		Crate* movedBottom = movedTop;
		for (int i = 1; i < count; i++)
		{
			movedBottom = movedBottom->below;
		}
		top = movedBottom->below;
		movedBottom->below = target.top;
		target.top = movedTop;
	}
};

std::vector<Stack> buildStacks(std::vector<std::string>& desc, Cargo& pool)
{
	std::vector<Stack> stacks;
	auto it = desc.rbegin();
	const std::string& line = (*it);
	for (int i = 0; i < line.length(); i += 4)
	{
		int nameIndex = (i + 1);
		char id = line[nameIndex];
		stacks.emplace_back(id,pool);
	}
	it++;

	for (it; it != desc.rend(); it++)
	{
		const std::string& crateLine = (*it);
		for(int stack=0;stack<stacks.size();++stack)
		{
			Stack& target = stacks[stack];
			int nameIndex = ((stack*4) + 1);
			if (nameIndex < crateLine.length())
			{
				char crateName = crateLine[nameIndex];
				if (crateName >= 'A')
				{
					target.AddCrate(crateName, pool);
				}
			}
		}
	}
	return std::move(stacks);
}

std::tuple<int, int, int> buildMove(std::string& line)
{
	if (line.length() == 0) { return { 0,0,0 }; };
	
	int count = 0;
	int from = 0;
	int to = 0;
	std::stringstream in(line);
	std::string item;
	std::getline(in, item, ' '); //'move'
	std::getline(in, item, ' '); //count
	count = std::stoi(item);
	std::getline(in, item, ' '); //'from'
	std::getline(in, item, ' '); //from
	from = std::stoi(item);
	std::getline(in, item, ' '); //'to'
	std::getline(in, item, '\n');//to
	to = std::stoi(item);
	
	return { count,from-1,to-1 };
}

void aoc2022_05()
{
	fs::path input(DataDir() / "2022_05.txt");
	std::ifstream inFile(input);
	Cargo cargo;

	__int64 score = 0;
	int readMoves = 0;
	std::vector<std::string> crateDescription;
	std::vector<std::tuple<int, int, int>> moves;
	for (std::string line; std::getline(inFile, line); ) {
		if( readMoves==0 )
		{ 
			if (line.length() == 0)
			{ 
				readMoves = 1; 
				continue; 
			}
			crateDescription.emplace_back(line);
		}
		else
		{
			if (line.length() == 0) { break; }
			moves.emplace_back(buildMove(line));
		}
	}

	std::vector<Stack> stacks= buildStacks(crateDescription, cargo);
	std::cout << stacks.size() << " Stacks\n";
	std::cout << moves.size() << " moves\n";
	
	for (const auto& step : moves)
	{
		int count = std::get<0>(step);
		int from = std::get<1>(step);
		int to = std::get<2>(step);

		Stack& source = stacks[from];
		Stack& target = stacks[to];
		//Part1, singleCrate
		/*
		for (int i = 0; i < count; ++i)
		{
			source.MoveTopTo(target);
		}
		*/
		source.MoveCountTo(count, target);
	}

	for (Stack& s : stacks)
	{
		std::cout << s.top->symbol;
	}
	std::cout << "\n";
	
}