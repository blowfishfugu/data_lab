#include "DataDir.h"
#include <iostream>

namespace {

	struct NumNode
	{
		__int64 val = 0LL;
		NumNode* next = nullptr;
		NumNode* prev = nullptr;
		NumNode(__int64 v):val(v){}
	};

	struct Arrangement
	{
		std::vector<NumNode> nodes;
		NumNode* first=nullptr;
		NumNode* last = nullptr;
		NumNode* zero = nullptr;
		std::vector<NumNode*> order;
		void init()
		{
			for (__int64 i=1;i<nodes.size();++i)
			{
				NumNode& prev = nodes[i-1];
				NumNode& current = nodes[i];
				order.push_back(&prev);
				current.prev = &prev;
				prev.next = &current;
				if (current.val == 0)
				{
					zero = &current;
				}
				else if (prev.val == 0)
				{
					zero = &prev;
				}
			}
			
			last= &nodes[nodes.size()-1];
			order.push_back(last);
			first= &nodes[0];
			last->next = first;
			first->prev = last;
		}
	};
}

void aoc2022_20()
{
	Arrangement arr;

	fs::path input(DataDir() / "2022_20.txt");
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0) { break; }
		arr.nodes.emplace_back(std::stoll(line));
	}
	arr.init();
	
	for (NumNode* current : arr.order)
	{
		__int64 steps = current->val;
		if (steps == 0)
		{
			break;
		}
		if (steps > 0)
		{
			NumNode* insertAt = current;
			while (steps > 0)
			{
				insertAt = insertAt->next;
				steps--;
			}
			
				current->prev->next = current->next;
				current->next->prev = current->prev;

				current->next = insertAt->next;
				current->prev = insertAt;
				current->next->prev = current;
				insertAt->next = current;
				
			
		}
		else if (steps < 0)
		{
			NumNode* insertAt = current;
			while (steps < 0)
			{
				insertAt = insertAt->prev;
				steps++;
			}
			
				current->prev->next = current->next;
				current->next->prev = current->prev;

				current->prev = insertAt->prev;
				current->next = insertAt;
				current->prev->next = current;
				insertAt->prev = current;

			
		}

		NumNode* n=arr.first;
		while (n->next != arr.first)
		{
			std::cout << n->val << ", ";
			n = n->next;
		}
		std::cout << n->val;
		std::cout << "\n";
	}

	__int64 step1000 = 1000%arr.order.size();
	__int64 step2000 = 2000 % arr.order.size();
	__int64 step3000 = 3000 % arr.order.size();

	NumNode* z1000 = arr.zero;
	while (step1000 > 0)
	{
		z1000 = z1000->next;
		step1000--;
	}
	NumNode* z2000 = arr.zero;
	while (step2000 > 0)
	{
		z2000 = z2000->next;
		step2000--;
	}
	NumNode* z3000 = arr.zero;
	while (step3000 > 0)
	{
		z3000 = z3000->next;
		step3000--;
	}

	__int64 score = z1000->val+z2000->val+z3000->val;
	std::cout << score << "\n"; //24669 "your answer is too high"
}