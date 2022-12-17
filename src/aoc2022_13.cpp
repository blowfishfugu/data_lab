#include "DataDir.h"
#include <iostream>
#include <array>
#include <stack>
#include <vector>
#include <variant>

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
		item* current = nullptr; //roots.top()
		item* head = nullptr;
		for (int i=0;i<line.size();)
		{
			char c = line[i];
			if(c=='[')
			{
				//roots.top()->list.push_back(new item);
				//roots.push(roots.top()->list[at size-1])...
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
		//std::cout << left->list.size() <<" " << right->list.size() << "\n";
		int res = (comp(left, right));
		if(res==1)
		{
			//std::cout << index << " right order\n";
			score += index;
		}
		else if (res == 0)
		{
			//std::cout << index << " wrong order\n";
		}
		else
		{
			//std::cerr << index << " !continue???\n";
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



//--adeccs template-Knoten.. variant, der Liste aus gleichem Type enthält
template<typename ty>
using VarTy = std::variant<size_t, std::vector<ty>>;

//Knoten
template <template <typename> class K>
struct Fixed : K<Fixed<K>> {
	using K<Fixed>::K;
};

using param_type = Fixed<VarTy>; // : K = VarTy<Fixed<VarTy>>
using param_list = std::vector<param_type>;

int compare(const param_type& lhs, const param_type& rhs)
{
	if (std::holds_alternative<size_t>(lhs) && std::holds_alternative<size_t>(rhs))
	{
		size_t l = std::get<size_t>(lhs);
		size_t r = std::get<size_t>(rhs);
		if (l == r) { return 2; } //equal
		if (l < r) { return 1; } //asc
		return 0; //desc
	}
	else if (std::holds_alternative<size_t>(rhs))
	{
		param_list list{ std::get<size_t>(rhs) };
		param_type tmp = list;
		return compare(lhs, tmp);
	}
	else if (std::holds_alternative<size_t>(lhs))
	{
		param_list list{ std::get<size_t>(lhs) };
		param_type tmp = list;
		return compare(tmp, rhs);
	}
	else
	{
		param_list const& lstLeft = std::get<param_list>(lhs);
		param_list const& lstRight = std::get<param_list>(rhs);
		for (auto[l, r] = std::make_pair(lstLeft.begin(), lstRight.begin());
			l != lstLeft.end() && r != lstRight.end();
			++l, ++r)
		{
			if (auto val = compare(*l, *r); val != 0) return val;
		}
		auto max_size = std::pair(lstLeft.size(), lstRight.size());
		return compare(max_size.first, max_size.second);
	}
	return 2;
}