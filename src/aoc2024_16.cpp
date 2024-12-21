#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <print>
#include <map>
#include <bitset>
#include <array>
#include <deque>
#include <set>
using namespace std::chrono_literals;

namespace D16 {
	using I = std::int64_t;
	struct V {
		I x{};
		I y{};
		V operator-(V o) const noexcept { return { x - o.x,y - o.y }; }
		V operator+(V o) const noexcept { return { x + o.x,y + o.y }; }
		V operator*(I s) const noexcept {
			I x_ = x * s; I y_ = y * s; return V{ x_,y_ };
		}
		V& operator+=(V o) noexcept { x += o.x; y += o.y; return *this; };
		auto operator<=>(const V& r) const = default;
	};
	using FieldFlags = std::bitset<4>;
	struct Field {
		char c{ '#' };
		FieldFlags entries{"0000"};
		FieldFlags exitsVisited{"1111"};
		I deerCount{};
		std::set<I> knownScores{};
	};
	using Row = std::vector<Field>;
	using Grid = std::vector<Row>;

	void clearScreen() {
		// CSI[2J clears screen, CSI[H moves the cursor to top-left corner
		std::cout << "\x1B[2J\x1B[H";
	}

	void print(const Grid& g) {
		std::ostringstream os;
		for (const Row& r : g) {
			for (const Field& f : r) {
				if (f.deerCount > 0) {
					std::print(os, "{}", f.deerCount);
				}
				else {
					std::print(os, "{}", f.c);
				}
			}
			std::print(os, "\n");
		}
		std::print(os, "\n\n");
		clearScreen();
		std::cout << os.str();
		std::this_thread::sleep_for(1ms);
	}



	enum DirId{N,E,S,W};
	struct DirData {
		char lbl{ ' ' };
		V dir{};
		DirId opposite{};
		DirId left{};
		DirId right{};
	};
	const static std::array<DirData,4> Directions{
		DirData{ 'N', V{ 0, -1}, S, W, E},
		DirData{ 'E', V{ 1, 0}, W, N, S},
		DirData{ 'S', V{ 0, 1}, N, E, W},
		DirData{ 'W', V{ -1,0}, E, S, N},
	};

	struct Deer {
		V curPos{};
		V dir{};
		DirId dirid{};
		I rotations{};
		I cost() { 
			V lastDir{Directions[E].dir};
			rotations = 0;;
			for (int i = 1; i < path.size();++i) {
				V prev = path[i - 1];
				V curr = path[i];
				V dir = curr - prev;
				if (dir != lastDir) {
					rotations++;
					lastDir = dir;
				}
				
			}

			return path.size() + rotations * 1000LL;
		}
		std::vector<V> path{};
		//I subSteps{};

		Field& GetRight(Grid& g, DirId& fieldEntry) const {
			const DirId right = Directions[dirid].right;
			const V& toRight = Directions[right].dir;
			fieldEntry = Directions[right].opposite;
			const V pRight = curPos + toRight;
			return g[pRight.y][pRight.x];
		}

		Field& GetLeft(Grid& g, DirId& fieldEntry) const {
			const DirId left = Directions[dirid].left;
			const V& toLeft = Directions[left].dir;
			fieldEntry = Directions[left].opposite;
			const V pLeft = curPos + toLeft;
			return g[pLeft.y][pLeft.x];
		}

		Field& GetFront(Grid& g, DirId& fieldEntry) const {
			const V pFront = curPos + dir;
			fieldEntry = Directions[dirid].opposite;
			return g[pFront.y][pFront.x];
		}

		void RotateRight() {
			this->dirid= Directions[dirid].right;
			this->dir = Directions[dirid].dir;
			++rotations;
		}
		void RotateLeft() {
			this->dirid = Directions[dirid].left;
			this->dir = Directions[dirid].dir;
			++rotations;
		}

		bool hasVisited(const V& pos) const {
			if (std::find(path.begin(), path.end(), pos) != path.end())
			{
				return true;
			}
			return false;
		}

		bool Step(Grid& g) {
			Field& last = g[curPos.y][curPos.x];
			--last.deerCount;
			//last.exitsVisited.set(dirid, true);
			//last.entries.set(dirid,false);
		
			curPos+= dir;
			if (hasVisited(curPos))
			{
				return false;
			}

			path.push_back(curPos);
			Field& next = g[curPos.y][curPos.x];
			
			++next.deerCount;
			return true;
			//++subSteps;
		}
		//sus
		/*void DeathEnd(Grid& g) {
			while (subSteps > 0 && path.size()>0) {
				const V& pos = *path.rbegin();
				Field& wall = g[pos.y][pos.x];
				wall.deerCount = 0;
				wall.c = 'X';
				wall.entries = "0000";
				wall.exitsVisited="1111";
				path.pop_back();
				--subSteps;
			}
			subSteps = 0;
		}*/
	};

	std::vector<Deer> startDeers(const V& sPos, const V& ePos, Grid& g) {
		std::vector<Deer> reachedEnd{};
		I highscore{std::numeric_limits<I>::max()};

		Deer first{};
		first.curPos = sPos;
		first.dirid = E;
		first.dir = Directions[first.dirid].dir;

		std::deque<Deer> active;
		active.push_back(first);

		while (active.size() > 0) {
			

			Deer& d = active.front();
			
			//for (const auto& v : d.path) {
			//	g[v.y][v.x].c = 'o';
			//}
			//print(g);
			//for (const auto& v : d.path) {
			//	g[v.y][v.x].c = '.';
			//}

			Field& current = g[d.curPos.y][d.curPos.x];
			DirId initialDir = d.dirid;
			DirId fEntry{};
			DirId lEntry{};
			DirId rEntry{};
			Field& forward = d.GetFront(g,fEntry);
			Field& left = d.GetLeft(g,lEntry);
			Field& right = d.GetRight(g,rEntry);

			bool canForward = (forward.entries.test(fEntry) );
			bool canLeft = (left.entries.test(lEntry) );
			bool canRight = (right.entries.test(rEntry) );
			
			if (!canForward && !canLeft && !canRight) {
				g[d.curPos.y][d.curPos.x].deerCount--;
				active.pop_front();
				continue;
			}
			
			if (canLeft) {
				if (!canForward)
				{
					d.RotateLeft();
				}
				else{
					//spawn + rotate + subSteps=0
					Deer spawn = d;
					spawn.dirid = initialDir;
					spawn.dir = Directions[initialDir].dir;
					spawn.RotateLeft();
					g[spawn.curPos.y][spawn.curPos.x].deerCount++;
					spawn.Step(g);
					active.emplace_back(spawn);

				}
			}

			if (canRight) {
				if (!canForward && !canLeft)
				{
					d.RotateRight();
				}
				else{
					//spawn + rotate + substeps=0
					Deer spawn = d;
					spawn.dirid = initialDir;
					spawn.dir = Directions[initialDir].dir;
					spawn.RotateRight();
					g[spawn.curPos.y][spawn.curPos.x].deerCount++;
					spawn.Step(g);
					active.emplace_back(spawn);
				}
			}

			if (canForward)
			{
				if (!d.Step(g))
				{
					active.pop_front();
				}
				if (d.curPos == ePos) {
					reachedEnd.push_back(d);
					active.pop_front();
					highscore = std::min(d.cost(), highscore);
					continue;
				}
				//if (d.cost() > highscore) {
				//	active.pop_front();
				//	continue;
				//}
			}

			
		}

		return reachedEnd;
	}

	
}

void aoc2024_16()
{
	using V = D16::V;
	using I = D16::I;
	using FieldFlags = D16::FieldFlags;
	using Field = D16::Field;
	using Row = D16::Row;
	using Grid = D16::Grid;
	using Deer = D16::Deer;

	fs::path input(DataDir() / "2024_16.txt");
	TxtFile txt{ input };

	I w{};
	I h{};
	V ePos{};
	V sPos{};
	Grid grid{};
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		Row row{};
		for (I x{}; char c : line) {
			if (c == '#') {
				row.push_back(Field{ '#',FieldFlags{"0000"},FieldFlags{"1111"},0LL });
			}
			if (c == 'S') {
				sPos = { x,h };
				row.push_back(Field{ 'S',FieldFlags{"1111"},FieldFlags{"0000"},0LL });
			}
			if (c == 'E') {
				ePos = { x,h };
				row.push_back(Field{'E',FieldFlags{"1111"},FieldFlags{"0000"},0LL });
			}
			if (c == '.') {
				row.push_back(Field{ '.', FieldFlags{"1111"},FieldFlags{"0000"},0LL });
			}
			++x;
		}
		w = row.size();
		grid.emplace_back(row);
		++h;
	}
	D16::print(grid);

	std::vector<D16::Deer> reachedEnd=D16::startDeers(sPos, ePos, grid);

	D16::print(grid);

	using I = D16::I;

	std::sort(reachedEnd.begin(), reachedEnd.end(), [](Deer& l, Deer& r) {
		return l.cost() > r.cost();
		});

	for (D16::Deer& deer : reachedEnd)
	{
		for (const auto& v: deer.path) {
			grid[v.y][v.x].c = 'o';
		}
		D16::print(grid);
		std::println(std::cout, "steps:{} rots:{} score:{}", deer.path.size(), deer.rotations, deer.cost());
		for (const auto& v : deer.path) {
			grid[v.y][v.x].c = '.';
		}
	}
}

