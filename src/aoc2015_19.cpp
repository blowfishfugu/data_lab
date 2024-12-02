#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "SplitIterator.h"
#include <iostream>
#include <cassert>

void aoc2015_19()
{
	fs::path input(DataDir() / "2015_19.txt");
	TxtFile txt{ input };
	using SV = std::string_view;
	using Rule = std::tuple<SV, SV>;
	std::vector<Rule> rules;
	SV hoho;

	__int64 state = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { state++; continue; }
		if (state == 0)
		{
			SpaceIterator it(line);
			SpaceIterator itVal(line);
			++itVal; ++itVal;
			rules.emplace_back(*it, *itVal);
		}
		if (state == 1)
		{
			hoho = line;
			++state;
		}
		if (state > 1)
		{
			break;
		}
	}

	struct Node {
		char C;
		int iteration{};
		explicit Node(char c): C(c){}
		std::vector<Node*> next{};

		void print(std::string indent) {
			std::cout << indent << C << "\n";
			for (Node* ch : next)
			{
				ch->print(indent + "-");
			}
		}
	};

	struct Lookup {
		std::vector<Node*> pool;
		Lookup() {};
		~Lookup() {
			for (Node* n : pool) { delete n; }
			pool.clear();
		}

		Node base{'*'};
		Node* Insert(const SV& str, Node* at, int iteration)
		{
			if (at == nullptr) {
				at = &base;
			}
			Node* last = at;
			for (const char c : str) {
				Node* next = nullptr;
				for (Node* n : at->next){
					if (n->C == c)
					{
						next = n;
						break;
					}
				}

				if(next==nullptr)
				{
					next = new Node(c);
					pool.push_back(next);

					next->iteration = iteration;
					at->next.push_back(next);
				}
				at = next;
				last = at;
			}
			return last;
		}
	};

	Lookup trie;
	//trie.Insert(hoho, nullptr, 0);

	for (const auto& [from, to] : rules)
	{
		size_t foundAt = hoho.find(from, 0LL);
		while (foundAt != SV::npos)
		{
			SV left = hoho.substr(0, foundAt);
			SV right = hoho.substr(foundAt + from.size());
			Node* start=trie.Insert(left, nullptr, 0);
			Node* end = trie.Insert(to, start, right.size()?0:1);
			Node* done = trie.Insert(right, end, 1);

			foundAt = hoho.find(from, foundAt+1);
		}
	}

	//trie.base.print("");
	auto branchCount=std::count_if(trie.pool.cbegin(), trie.pool.cend(),
		[](const Node* n) {
			return (n->next.size() == 0) && (n->iteration==1);
		}
	);

	std::cout << std::format("branches: {}\n", branchCount);
}

