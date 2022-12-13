#include "DataDir.h"
#include <iostream>
#include <array>
#include <stack>
#include <vector>

struct item
{
	int integer = 0;
	std::vector<const item*> list;
	int type = 0;//list
};

int comp (const item* left, const item* right) //1 ASC, 0 DESC, 2=EQUAL continue //TODO: operator< überladen?
{
	if (left->type == 1 && right->type == 1)
	{
		if (left->integer < right->integer)
		{
			return 1;
		}
		else if (left->integer > right->integer)
		{
			return 0;
		}
		return 2;
	}
	if (left->type == 0 && right->type == 0)
	{
		int i = 0;
		for (i = 0;
			i < left->list.size() && i < right->list.size();
			++i)
		{
			int res = comp(left->list[i], right->list[i]);
			if (res == 2) { continue; }
			if (res == 1) { return 1; }
			if (res == 0) { return 0; }
		}
		
		if ( left->list.size() != right->list.size())
		{
			if (left->list.size() < right->list.size())
			{
				return 1;
			}
			if (right->list.size() < left->list.size())
			{
				return 0;
			}
		}
		return 2;
	}
	if (left->type == 0 && right->type == 1)
	{
		item tmp;
		tmp.type = 0;
		tmp.list.push_back(right);
		return comp(left, &tmp);
	}
	else if (left->type == 1 && right->type == 0)
	{
		item tmp;
		tmp.type = 0;
		tmp.list.push_back(left);
		return comp(&tmp,right);
	}
	return 1;
}

void aoc2022_13()
{
	fs::path input(DataDir() / "2022_13.txt");
	std::ifstream inFile(input);
	

	std::vector<item*> pool;
	auto readPackets=[&pool](std::string& line)
	{
		std::stack<item*> roots;
		item* current = nullptr;
		item* head = nullptr;
		for (int i=0;i<line.size();)
		{
			char c = line[i];
			if(c=='[')
			{
				item* lastCurrent = current;
				current = new item;
				pool.push_back(current);
				if (head == nullptr)
				{
					head = current;
				}
				roots.push(current);
				current = roots.top();
				if (lastCurrent != nullptr)
				{
					lastCurrent->list.push_back(current);
				}
				++i;
			}
			else if (c == ']')
			{
				roots.pop();
				if (roots.size() > 0)
				{
					current = roots.top();
				}
				else
				{
					std::cerr << "unequal [] pairs!!\n";
				}
				++i;
			}
			else
			{
				std::string str;
				while (c != ']' && c != ',')
				{
					str.push_back(c);
					++i;
					c = line[i];
				}
				if (c == ',')
				{
					++i;
				}
				//read to comma or ]
				if (str.length() > 0)
				{
					int val = std::stoi(str);
					item* valItem = new item;
					pool.push_back(valItem);
					valItem->integer = val;
					valItem->type = 1;
					current->list.push_back(valItem);
				}
			}
		}
		return head;
	};
	
	std::vector<std::string> lines;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { continue; }
		lines.emplace_back(line);
	}

	std::vector<item*> roots;
	
	__int64 score = 0;
	int index = 1;
	for (int i = 1; i < lines.size(); i += 2)
	{
		item* left = readPackets(lines[i-1]);
		item* right = readPackets(lines[i]);
		roots.push_back(left);
		roots.push_back(right);
		std::cout << left->list.size() <<" " << right->list.size() << "\n";
		int res = (comp(left, right));
		if(res==1)
		{
			std::cout << index << " right order\n";
			score += index;
		}
		else if (res == 0)
		{
			std::cout << index << " wrong order\n";
		}
		else
		{
			std::cerr << index << " !continue???\n";
		}
		index++;
	}


	std::cout << "sum of right " << score << "\n";

	std::string d2("[[2]]");
	std::string d6("[[6]]");
	item* div2 = readPackets(d2);
	item* div6 = readPackets(d6);
	roots.push_back(div2);
	roots.push_back(div6);

	std::sort(roots.begin(), roots.end(), [](item* l, item* r) {
		return comp(l, r)==1;
		});
	index = 1;
	__int64 i2 = 0;
	__int64 i6 = 0;
	for(item* tst:roots)
	{ 
		if (tst == div2)
		{
			i2 = index;
		}
		else if (tst == div6)
		{
			i6 = index;
		}
		index++;
	}
	std::cout << i2 * i6 << "\n";

	for (item*& d : pool) { delete d; d = nullptr; }
}