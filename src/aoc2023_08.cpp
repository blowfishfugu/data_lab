#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <map>
#include <numeric>
#include <array>

namespace D08
{
	

}

void aoc2023_08()
{
	fs::path input(DataDir() / "2023_08.txt");
	TxtFile txt{ input };

	std::vector<size_t> nexts{};

	struct Node
	{
		std::string lbl{};
		std::array<Node*,2> lr{nullptr,nullptr};
	};
	std::map<std::string, Node*> nodepool;
	auto getCreate=[&nodepool]( std::string_view lbl )
	{
			auto it = nodepool.find( std::string{ lbl } );
			if( it != nodepool.end() ) { return ( *it ).second; }
			Node* n = new Node();
			n->lbl = lbl;
			nodepool.emplace( lbl, n );
			return n;
	};

	__int64 count = 0LL;
	int inputstate{};
	for (const auto& line : txt)
	{
		if (line.length() == 0 && inputstate==2 ) { break; }
		if( inputstate == 0 )
		{
			for (char LR : line)
			{
				if(LR=='L') nexts.emplace_back(0LL);
				else if( LR=='R') nexts.emplace_back(1LL);
			}
			inputstate = 1;
			++count;
		}
		else if( inputstate == 1 )
		{
			if( line.length() == 0 ) { continue; }
			inputstate = 2;
		}

		if( inputstate==2 )
		{
			auto [lbl, childs] = pairSplit<'=', true>( line );
			trim<' '>( lbl );
			trim<' '>( childs );
			trim<'('>( childs );
			trim<')'>( childs );
			
			Node* parent = getCreate( lbl );
			auto [l, r] = pairSplit<',', true>( childs );
			trim<' '>( l );
			trim<' '>( r );

			Node* childL = getCreate( l );
			Node* childR = getCreate( r );
			parent->lr[0]= childL;
			parent->lr[1]= childR;
			++count;
		}
	}
	std::cout << "count: " << count << "\n";
	
	int pos = 0;
	__int64 step = 0;
	if( nodepool.find( "AAA" ) != nodepool.end() )
	{
		Node* current = getCreate( "AAA" );

		while( current->lbl != "ZZZ" )
		{
			step++;
			current = current->lr[ nexts[pos] ];
			pos++;
			if( pos >= nexts.size() ) { pos = 0; }
		}
		std::cout << "steps AAA-ZZZ: " << step << "\n";
	}
	assert(step == 13207 || step == 6 || step == 2);

	std::vector<Node*> ghosts;
	for( auto& [lbl,node]: nodepool)
	{ 
		if (lbl[2] == 'A') {
			std::cout << "ghost start: " << lbl << "\n";
			ghosts.push_back(node); 
		}
	}

	//first try was bruteforce
	auto allHaveZ = []( const std::vector<Node*>& tst ) {
		for( const Node* n : tst ) {
			if( n->lbl[ 2 ] != 'Z' ) { return false; } 
		}
		return true;
		};

	std::vector<__int64> deltas;
	if( ghosts.size() > 0 )
	{
		for (Node* current : ghosts)
		{
			pos = 0;
			step = 0;
			__int64 laststep = step;
			for (int i = 0; i < 1; ++i) //loop to check deltas between hits
			{
				laststep = step;
				while (true)
				{
					step++;
					current = current->lr[nexts[pos]];
					pos++;
					if (pos >= nexts.size()) { pos = 0; }
					if (allHaveZ({current}))
					{
						break;
					}
				}
				std::cout << "ghoststeps: " << step << "\tpos: " << pos << "\tdelta: " << step - laststep << "\n";
			}
			deltas.push_back(step-laststep);
		}
	}

	__int64 minimumLoop =deltas[0];
	for (size_t i = 1; i < deltas.size(); i++)
	{
		minimumLoop = std::lcm(minimumLoop, deltas[i]);
		std::cout << (i+1) << "Zs at: " << minimumLoop << "\n";
	}

	for( auto& [_, node] : nodepool )
	{
		delete node; node = nullptr;
	}
	nodepool.clear();

	assert(minimumLoop == 12324145107121 || minimumLoop == 6);
}

