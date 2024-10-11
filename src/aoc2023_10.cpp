#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <array>
#include <algorithm>
#include <numeric>
#include <map>
#include <sstream>
#include <deque>

namespace D10 {
	enum LRTB : int{
		X=0b0000,
		L=0b1000,
		R=0b0100,
		T=0b0010,
		B=0b0001,
	};
	// │ ─ ┘ ┐ └ ┌ · ☺
	constexpr std::array<unsigned char, 8> labels{ 0xb3, 0xc4, 0xd9,0xbf,0xc0,0xda,0xfa,0x01 };
	
	static const std::map<int, unsigned char> signatures{
		{T|B, labels[0]},
		{L|R, labels[1]},
		{T|L, labels[2]},
		{B|L, labels[3]},
		{T|R, labels[4]},
		{B|R, labels[5]},
		{X&0, labels[6]},
		{L|R|T|B, labels[7]},
	};

	static const std::map<char, int> inputSymbols{
		{'|',T | B},
		{'-',L | R},
		{'7',B | L},
		{'J',T | L},
		{'F',B | R},
		{'L',T | R},
		{'.',X & 0},
		{'S', L|R|T|B}
	};
	
	template<bool verbose>
	void buildLabels()
	{
		if constexpr (verbose)
		{
			std::array<unsigned char, 256 - 1> specials{};
			std::iota(specials.begin(), specials.end(), 1);
			for (int i = 0; unsigned char c : specials)
			{
				std::cout << std::format("{} {:X}  ", (char)c, c);
				++i;
				i = ((i % 8) == 0) ? (std::cout << '\n' && 0) : i;
			}
			std::cout << "\n";
		}

		for (int i = 0, y = 0; y < 6; ++y)
		{
			for (char c : labels)
			{
				std::cout << std::format("{}", c);
				++i;
				i = ((i % 12) == 0) ? (std::cout << '\n' && 0) : i;
			}
		}
		std::cout << "\n";
	}

	struct Pipe {
		unsigned char lbl{ '.' };
		int signature{X};
		__int64 step{-1};
		bool isPath{ false };
		int x{};
		int y{};
		std::array<Pipe*,4> lrtb{};
		
	};

	struct Grid final{
		using Row = std::vector<Pipe*>;
		using Items = std::vector<Row>;
		int lBound{};
		int rBound{};
		int tBound{};
		int bBound{};
		Pipe* S{};
		Items items{};
		std::vector<Pipe*> leftTBs;

		~Grid(){
			for (Row& row : items)
			{
				for (Pipe*& p : row)
				{
					delete p; p = nullptr;
				}
			}
			items.clear();
		}
	
		template<typename out, typename in>
		out tryFind( const std::map<in, out>& m, in search, out defaultValue)
		{
			out val = defaultValue;
			auto it = m.find(search);
			if (it != m.end())
			{
				val = it->second;
			}
			return val;
		}

		void read(const fs::path& input)
		{
			TxtFile txt{ input };
			bBound = 0;
			for (int posY=0; const auto& line : txt)
			{
				if (line.length() == 0) { break; }
				if (rBound == 0) { rBound = static_cast<int>(line.length()); }
				++bBound;
				Row row;
				Pipe* lefter = nullptr;
				for (int posX=0; char i : line)
				{
					int signature = tryFind<int,char>( inputSymbols, i, X );
					unsigned char lbl = tryFind<unsigned char, int>( signatures, signature, labels[6] );
					Pipe* p = new Pipe;
					p->x = posX;
					posX++;
					p->y = posY;
					p->lbl = lbl;
					p->signature = signature;
					if (p->signature == (L | R | T | B))
					{
						S = p; S->step = 0; S->isPath = true;
					}
					if (lefter==nullptr && p->signature == (T | B))
					{
						lefter = p;
						leftTBs.push_back(p);
					}
					row.emplace_back(p);
				}
				items.emplace_back(row);
				posY++;
			}
			std::cout << "count: " << bBound << "\n";
		}
	
		void connect()
		{
			for (int y = 0; y < bBound; ++y)
			{
				for (int x = 0; x < rBound; ++x)
				{
					Pipe* current = items[y][x];
					int sigCurrent = current->signature;

					if (x > 0)
					{
						Pipe* left = items[y][x - 1];
						int sigLeft = left->signature;
						if (((sigCurrent & LRTB::L) == LRTB::L)
							&& ((sigLeft & LRTB::R) == LRTB::R))
						{
							left->lrtb[1] = current; current->lrtb[0] = left;
							left->step = 0; current->step = 0;
						}
					}
					
					if (y < (items.size()-1))
					{
						Pipe* below = items[y + 1][x];
						int sigBelow = below->signature;

						if (((sigCurrent & LRTB::B) == LRTB::B)
							&& ((sigBelow & LRTB::T) == LRTB::T))
						{
							below->lrtb[2] = current; current->lrtb[3] = below;
							below->step = 0; current->step = 0;
						}
					}
				}
			}
		}

		void print()
		{
			__int64 overall{};
			__int64 outerOrPipe{};
			for (const Row& row : items)
			{
				std::stringstream os;
				for (const Pipe* p : row)
				{
					os << (char)p->lbl;
					if (p->isPath || p->lbl == 'O')
					{
						++outerOrPipe;
					}
					++overall;
				}
				os << "\n";
				std::cout << os.str();
			}
			std::cout << std::format("{} - {} = {}\n", overall,outerOrPipe, overall-outerOrPipe);
		}

		void resetSteps()
		{
			for (const Row& row : items)
			{
				for (Pipe* p : row)
				{
					if (p->isPath) { p->step = 0; }
					else { p->lbl = labels[6]; }
				}
			}
		}
	};

}

void aoc2023_10()
{
	using namespace D10;
	buildLabels<false>();
	fs::path input(DataDir() / "2023_10.txt");
	Grid grid;
	grid.read(input);
	grid.connect();
	grid.print();

	struct Rabbit {
		Pipe* pos{};
		Pipe* lastpos{};
		bool Step() {
			bool didStep = false;
			for( int dir=0;dir<pos->lrtb.size();++dir)
			{
				Pipe* next = pos->lrtb[dir];
				if (next == nullptr) { continue; }
				if (next->step > pos->step) { continue; }
				if (next == lastpos) { continue; }
				next->step=pos->step+1;
				lastpos = pos;
				pos = next;
				pos->isPath = true;
				didStep = true;
				break;
			}
			return didStep;
		}
	};

	std::array<Rabbit, 2> rabbits;
	rabbits[0].pos = grid.S;
	rabbits[1].pos = grid.S;
	while (true)
	{
		bool didstep = rabbits[0].Step() && rabbits[1].Step();
		if (!didstep)
		{
			break;
		}
	}
	grid.print();
	std::cout << "R0: " << rabbits[0].pos->step << "\n";
	std::cout << "R1: " << rabbits[1].pos->step << "\n";
	__int64 maxSteps = std::max(rabbits[0].pos->step, rabbits[1].pos->step);
	std::cout << "maxsteps: " << maxSteps << "\n";

	//Part2 mark inner-outer
	grid.resetSteps();
	grid.S->lbl = grid.tryFind(signatures, grid.S->signature, grid.S->lbl);

	auto newStart = std::min_element(grid.leftTBs.begin(), grid.leftTBs.end(), 
		[](const Pipe* l, const Pipe* r) { 
			if (!l->isPath && !r->isPath) { return false; }
			if (l->isPath && !r->isPath) { return true; }
			if (!l->isPath && r->isPath) { return false; }
			return l->x < r->x;
		}
	);

	Pipe* newS = *newStart;
	newS->lbl = labels[7];
	grid.print();

	struct Marker {
		Grid& grid;
		Marker( Grid& g) : grid{g}{}
		int currentdir = 2;
		Pipe* initialPos{};
		Pipe* pos{};
		Pipe* lastpos{};
		void flood(Pipe* outer)
		{
			static std::vector<std::pair<int, int>> directions{
				{1,0},
				{-1,0},
				{0,1},
				{0,-1},
			};

			std::deque<Pipe*> todo;
			todo.push_back(outer);
			while (todo.size() > 0)
			{
				Pipe* tst = todo.front(); todo.pop_front();
				if (!tst->isPath && tst->lbl!='O')
				{
					tst->lbl = 'O';
					//break;
					for (const auto& [cx, cy] : directions)
					{
						int tstX = tst->x + cx;
						int tstY = tst->y + cy;
						if (tstX >= 0 && tstX < grid.rBound && tstY >= 0 && tstY < grid.bBound)
						{
							todo.push_back(grid.items[tstY][tstX]);
						}
					}
				}
			}
		}
		bool Step() {
			bool didStep = false;
			int cx{};
			int cy{};
			if (currentdir == 0) //L
			{
				cx = 0;
				cy = +1;
			}
			else if (currentdir == 1)//R
			{
				cx = 0;
				cy = -1;
			}
			else if (currentdir == 2)//T
			{
				cx = -1;
				cy = 0;
			}
			else if (currentdir == 3)//B
			{
				cx = 1;
				cy = 0;
			}

			int tstX = pos->x + cx;
			int tstY = pos->y + cy;
			if (tstX >= 0 && tstX < grid.rBound && tstY >= 0 && tstY < grid.bBound)
			{
				Pipe* outer = grid.items[tstY][tstX];
				flood(outer);
			}
			
			for (int dir = 0; dir < pos->lrtb.size(); ++dir)
			{
				Pipe* next = pos->lrtb[dir];
				if (next == nullptr) { continue; }
				if (next == lastpos) { continue; }
				if (next == initialPos)
				{ 
					continue; 
				}
				lastpos = pos;
				didStep = true;

				if (currentdir == 2 && dir == 1) //F -> next== (B|R) ?
				{
					int tstX = pos->x + 0;
					int tstY = pos->y - 1;
					if (tstX >= 0 && tstX < grid.rBound && tstY >= 0 && tstY < grid.bBound)
					{
						Pipe* outer = grid.items[tstY][tstX];
						flood(outer);
					}
				}
				if (currentdir == 2 && dir == 1) //7
				{
					int tstX = pos->x + 0;
					int tstY = pos->y + 1;
					if (tstX >= 0 && tstX < grid.rBound && tstY >= 0 && tstY < grid.bBound)
					{
						Pipe* outer = grid.items[tstY][tstX];
						flood(outer);
					}
				}
				if (currentdir == 3 && dir == 1) //L
				{
					int tstX = pos->x + 0;
					int tstY = pos->y - 1;
					if (tstX >= 0 && tstX < grid.rBound && tstY >= 0 && tstY < grid.bBound)
					{
						Pipe* outer = grid.items[tstY][tstX];
						flood(outer);
					}
				}
				if (currentdir == 3 && dir == 0) //J
				{
					int tstX = pos->x + 0;
					int tstY = pos->y + 1;
					if (tstX >= 0 && tstX < grid.rBound && tstY >= 0 && tstY < grid.bBound)
					{
						Pipe* outer = grid.items[tstY][tstX];
						flood(outer);
					}
				}
				
				pos = next;
				currentdir = dir;
				break;
			}
			return didStep;
		}
	};

	Marker m(grid);
	m.pos = newS;
	m.initialPos = newS;
	while (m.Step())
	{
		//grid.print();
	}
	grid.print();
}

