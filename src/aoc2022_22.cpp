#include "DataDir.h"
#include <iostream>
#include <array>
#include <map>
#include <tuple>
#include <functional>
#include <numeric>
namespace
{
	constexpr __int64 R = 0LL;
	constexpr __int64 D = 1LL;
	constexpr __int64 L = 2LL;
	constexpr __int64 U = 3LL;

	struct coord {
		__int64 x = 0LL; __int64 y = 0LL;
		coord(__int64 _x=0LL, __int64 _y=0LL)
			: x(_x),y(_y)
		{}

		friend bool operator <(const coord& lhs, const coord& rhs) noexcept {
			return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
		}
		friend bool operator >(const coord& lhs, const coord& rhs) noexcept {
			return std::tie(lhs.x, lhs.y) > std::tie(rhs.x, rhs.y);
		}
		friend bool operator ==(const coord& lhs, const coord& rhs)  noexcept {
			return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y);
		}
		friend bool operator !=(const coord& lhs, const coord& rhs)  noexcept {
			return std::tie(lhs.x, lhs.y) != std::tie(rhs.x, rhs.y);
		}
	};

	struct cell
	{
		coord pos{};
		char content = '.'; // . # x
		
		//0 R, 1 D, 2 L, 3 U
		std::array<cell*, 4> nexts={nullptr,nullptr,nullptr,nullptr}; 
	};


	using Map = std::map<coord, cell*>;

	[[nodiscard]]
	cell* getCreateCell(__int64 x, __int64 y, char lbl, Map& pool)
	{
		coord key{ x,y };
		auto f = pool.find(key);
		if (f != pool.end()) { return f->second; }
		cell* c = new cell;
		c->pos = key;
		c->content = lbl;
		pool.insert({ key, c });
		return c;
	}
	auto clear(Map& pool)
	{
		for (auto& kv : pool)
		{
			if (kv.second != nullptr)
			{
				delete kv.second;
				kv.second = nullptr;
			}
		}
		pool.clear();
	}
	using Command = std::pair<__int64, char>; //steps,rotation
	struct turtle
	{
		cell* currentPos = nullptr;
		__int64 direction = R;
		void mark()
		{
			char mark = 'x';
			switch (direction)
			{
			case(U):
				mark = '^'; break;
			case(D):
				mark = 'v'; break;
			case(L):
				mark = '<'; break;
			case(R):
				mark = '>'; break;
			default:
				break;
			}
			currentPos->content = mark;
		}
		void rotate(char LR)
		{
			if (LR == 'L') { direction--; if (direction < R) { direction = U; } }
			else if (LR == 'R') { direction++; if (direction > U) { direction = R; } }
			mark();
		}
		void move(__int64 steps)
		{
			while (steps > 0)
			{
				cell* next = currentPos->nexts[direction];
				if (next->content == '#')
				{
					break;
				}

				//über welche verbindung kommen wir rein
				__int64 comeFrom = 0;
				for(;comeFrom<next->nexts.size();comeFrom++)
				{ 
					cell* tst = next->nexts[comeFrom];
					if (tst == currentPos)
					{
						break;
					}
				}

				switch (comeFrom) //bei schritt über kante ist richtung verbindungsabhängig
				{
				case(U):
					direction = D; break;
				case(D):
					direction = U; break;
				case(L):
					direction = R; break;
				case(R):
					direction = L; break;
				default:
					break;
				}

				currentPos = next;
				mark();
				steps--;
			}
		}
		void apply(const Command& cmd)
		{
			rotate(cmd.second);
			move(cmd.first);
		}
	};

	//verbindet tops mit bottoms und lefts mit rights
	//dadurch planare "endlosmap"
	cell* readData(Map& board, std::vector<Command>& moves)
	{
		fs::path input(DataDir() / "2022_22.txt");
		std::ifstream inFile(input);
		//Map
		__int64 y = 1;
		std::map<__int64, cell*> tops;
		std::map<__int64, cell*> bottoms;
		cell* startCell = nullptr;
		for (std::string line; std::getline(inFile, line);)
		{
			if (line.length() == 0) { 
				break; 
			}
			
			cell* first = nullptr;
			cell* last = nullptr;
			for (__int64 x = 1; x <= line.length(); ++x)
			{
				char lbl = line[x - 1];
				if (lbl != ' ') {
					cell* c = getCreateCell(x, y, lbl, board);
					if (startCell == nullptr) { startCell = c; }
					if (first == nullptr) { first = c; }
					last = c;
					if (tops[x] == nullptr) { tops[x] = c; }
					bottoms[x] = c;

					cell* l = board[{x - 1, y}];
					c->nexts[L] = l;
					if (l) { l->nexts[R] = c; }
					cell* u = board[{x, y - 1}];
					c->nexts[U] = u;
					if (u) { u->nexts[D] = c; }
				}
			}
			
			if (last && first)
			{
				last->nexts[R] = first;
				first->nexts[L] = last;
			}
			y++;
		}

		for (auto& t : tops)
		{
			__int64 x = t.first;
			cell* top = tops[x];
			cell* bottom = bottoms[x];
			if (top&&bottom)
			{
				top->nexts[U] = bottom;
				bottom->nexts[D] = top;
			}
		}

		//directions
		std::string lastMove;
		for (std::string line; std::getline(inFile, line);)
		{
			if (line.length() == 0) { 
				break; 
			}
			for (__int64 i = 0; i < line.length(); i++)
			{
				char x = line[i];
				if( std::isdigit(x,std::locale("")))
				{ 
					lastMove.push_back(x);
				}
				else {
					if (lastMove.length() > 0)
					{
						moves.emplace_back(std::stoll(lastMove), 0);
						lastMove.clear();
					}
					moves.emplace_back(0, x);
				}
			}
		}
		if (lastMove.length() > 0)
		{
			moves.emplace_back(std::stoll(lastMove), 0);
			lastMove.clear();
		}

		return startCell;
	}

	//reissverschluss. 
	//startEckpunkte von Kanten verknüpfen, Links zu Up + jeweilige Richtung
	//steps, c1, connectL, walkUp, c2, connectU, walkL
	void connect(__int64 steps,
		cell* cFrom, __int64 nFrom, __int64 stepDirFrom, 
			cell* cTo, __int64 nTo, __int64 stepDirTo)
	{
		for (__int64 i = 0; i < steps; i++)
		{
			cFrom->nexts[nFrom] = cTo;
			cTo->nexts[nTo] = cFrom;

			cFrom = cFrom->nexts[stepDirFrom];
			cTo = cTo->nexts[stepDirTo];
		}
	}

	coord TL(__int64 tx, __int64 ty, __int64 tileDim)
	{
		return { tileDim * tx + 1, tileDim*ty+1 };
	}

	coord BL(__int64 tx, __int64 ty, __int64 tileDim)
	{
		return {tileDim*tx+1, tileDim*ty+tileDim};
	}

	coord TR(__int64 tx, __int64 ty, __int64 tileDim)
	{
		return { tileDim * tx + tileDim, tileDim*ty + 1 };
	}

	coord BR(__int64 tx, __int64 ty, __int64 tileDim)
	{
		return { tileDim*tx +tileDim, tileDim*ty + tileDim };
	}

	// Handverknüpft
	// alternative: jede Oberfläche einzeln indizieren
	// den direkten Nachbarn inkl daran hängender nachbarn um 90 Grad Biegen
	// zu jeder Verbindung (aus nexts), die null ist, den schwesternpunkt finden, dessen Abstand am nächsten liegt
	void connectEdges(Map& board, __int64 tileDim)
	{
		//.##
		//.#.
		//##.
		//#..
		{
			cell* c1 = board[TR(1, 1, tileDim)];
			cell* c2 = board[BL(2, 0, tileDim)];
			connect(tileDim, c1, R, D, c2, D, R);
		}
		{
			cell* c1 = board[BL(1, 1, tileDim)];
			cell* c2 = board[TR(0, 2, tileDim)];
			connect(tileDim, c1, L, U, c2, U, L);
		}
		{
			cell* c1 = board[BL(1, 2, tileDim)];
			cell* c2 = board[TR(0, 3, tileDim)];
			connect(tileDim, c1, D, R, c2, R, D);
		}
		{
			cell* c1 = board[BR(2, 0, tileDim)];
			cell* c2 = board[TR(1, 2, tileDim)];
			connect(tileDim, c1, R, U, c2, R, D);
		}
		{
			cell* c1 = board[TL(0, 3, tileDim)];
			cell* c2 = board[TL(1, 0, tileDim)];
			connect(tileDim, c1, L, D, c2, U, R);
		}
		{
			cell* c1 = board[TL(0, 2, tileDim)];
			cell* c2 = board[BL(1, 0, tileDim)];
			connect(tileDim, c1, L, D, c2, L, U);
		}
		{
			cell* c1 = board[BR(0, 3, tileDim)];
			cell* c2 = board[TR(2, 0, tileDim)];
			connect(tileDim, c1, D, L, c2, U, L);
		}
	}

	void connectEdges_example(Map& board, __int64 tileDim)
	{
		{
			cell* c1 = board[BL(2,0,tileDim)];
			cell* c2 = board[TR(1,1,tileDim)];
			connect(tileDim, c1, L, U, c2, U, L);
		}

		{
			cell* c1 = board[TL(2,0,tileDim)];
			cell* c2 = board[TR(0,1,tileDim)];
			connect(tileDim, c1, U, R, c2, U, L);
		}

		{
			cell* c1 = board[TL(2,2,tileDim)];
			cell* c2 = board[BR(1,1,tileDim)];
			connect(tileDim, c1, L, D, c2, D, L);
		}

		{
			cell* c1 = board[BL(2,2,tileDim)];
			cell* c2 = board[BR(0,1,tileDim)];
			connect(tileDim, c1, D, R, c2, D, L);
		}

		{
			cell* c1 = board[TR(2, 0, tileDim)];
			cell* c2 = board[BR(3, 2, tileDim)];
			connect(tileDim, c1, R, D, c2, R, U);
		}

		{
			cell* c1 = board[TL(3, 2, tileDim)];
			cell* c2 = board[BR(2, 1, tileDim)];
			connect(tileDim, c1, U, R, c2, R, U);
		}

		{
			cell* c1 = board[BL(0, 1, tileDim)];
			cell* c2 = board[BL(3, 2, tileDim)];
			connect(tileDim, c1, L, U, c2, D, R);
		}
	}

	cell* readData2(Map& board, std::vector<Command>& moves)
	{
		fs::path input(DataDir() / "2022_22.txt");
		std::ifstream inFile(input);
		//Map
		__int64 y = 0;
		__int64 dx = 0;
		__int64 dy = 0;
		cell* startCell = nullptr;
		for (std::string line; std::getline(inFile, line);)
		{
			if (line.length() == 0) {
				break;
			}

			y++;
			for (__int64 x = 1; x <= line.length(); ++x)
			{
				char lbl = line[x - 1];
				if (lbl != ' ') {
					cell* c = getCreateCell(x, y, lbl, board);
					if (c != nullptr)
					{
						if (startCell == nullptr) { startCell = c; }

						cell* l = board[{x - 1, y}];
						c->nexts[L] = l;
						if (l) { l->nexts[R] = c; }
						cell* u = board[{x, y - 1}];
						c->nexts[U] = u;
						if (u) { u->nexts[D] = c; }
					}
				}
			}
			dy = std::max(dy, y);
			dx = std::max(dx, (__int64)line.length());
		}

		__int64 tileDim=std::gcd(dx, dy);
		connectEdges(board, tileDim);
		

		//directions
		std::string lastMove;
		for (std::string line; std::getline(inFile, line);)
		{
			if (line.length() == 0) {
				break;
			}
			for (__int64 i = 0; i < line.length(); i++)
			{
				char x = line[i];
				if (std::isdigit(x, std::locale("")))
				{
					lastMove.push_back(x);
				}
				else {
					if (lastMove.length() > 0)
					{
						moves.emplace_back(std::stoll(lastMove), 0);
						lastMove.clear();
					}
					moves.emplace_back(0, x);
				}
			}
		}
		if (lastMove.length() > 0)
		{
			moves.emplace_back(std::stoll(lastMove), 0);
			lastMove.clear();
		}

		return startCell;
	}
}

void print(Map& board)
{
	for (__int64 y = 1; y <= 12; ++y)
	{
		std::string line;
		for (__int64 x = 1; x <= 16; ++x)
		{
			cell* c = board[{x, y}];
			if (!c) { line.push_back(' '); }
			else { line.push_back(c->content); }
		}
		std::cout << line.c_str() << "\n";
	}
	std::cout << "\n\n";
}

void aoc2022_22()
{
	Map board;
	std::vector<Command> directions;
	
	cell* start=readData2(board, directions);
	turtle self;
	self.currentPos = start;
	for (const Command& cmd : directions)
	{
		self.apply(cmd);
		//print(board);
	}

	//print(board);
	
	cell* endpos = self.currentPos;
	__int64 score = 1000LL*endpos->pos.y + 4LL*endpos->pos.x + self.direction;
	std::cout << score << "\n";
	
	clear(board);
}