#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <bitset>
#include <array>
#include <format>
#include <print>
#include <deque>

void aoc2024_12()
{
	fs::path input(DataDir() / "2024_12.txt");
	TxtFile txt{ input };
	
	using I = std::int64_t;

	struct Dir {
		I dx{};
		I dy{};
		Dir(I x = 0, I y = 0) : dx{ x }, dy{ y } {}
		Dir operator+(Dir d) { return { dx + d.dx,dy + d.dy }; }
	};
	enum DLabel {
		N,E,S,W
	};
	using Needle= std::tuple < Dir, DLabel, DLabel>;
	static std::array<Needle, 4> Directions{
		Needle{{ 0,-1}, N, S},
		Needle{{ 1, 0}, E, W},
		Needle{{ 0, 1}, S, N},
		Needle{{-1, 0}, W, E}
	};

	struct Plant{
		I x{};
		I y{};
		char name{};
		std::bitset<4> fence{ "1111" };
		bool observed{ false };
		std::bitset<4> fenceHandled{ "0000" };
	};
	using Row = std::vector<Plant>;
	using Garden = std::vector<Row>;
	struct Region {
		char name{};
		std::vector<Plant*> plants{};
		I fenceCount{};
		void fill(Plant& tst, Garden& garden, I w, I h) {
			auto inBounds = [w, h](const Dir& pos)->bool {
				if (pos.dx < 0 || pos.dx >= w) { return false; }
				if (pos.dy < 0 || pos.dy >= h) { return false; }
				return true;
				};
			
			std::deque<std::tuple<Dir,Needle>> toVisit{};
			toVisit.emplace_back(Dir{ tst.x,tst.y }, Needle{ Dir{}, N, N });

			while (toVisit.size() > 0)
			{
				auto [curPos,comeFrom] = toVisit.front(); toVisit.pop_front();
				const auto& [Navi, ForwardDir, ReverseDir] = comeFrom;
				Plant& current = garden[curPos.dy][curPos.dx];
				if (current.name != name) { 
					continue; 
				}

				//remove Fence from current and prev
				//todo, pass last visited cell instead
				if (Navi.dx!=0 || Navi.dy!=0 ) {
					Dir toPrev = curPos + std::get<0>(Directions[ReverseDir]);
					Plant& prevPlant = garden[toPrev.dy][toPrev.dx];
					prevPlant.fence.set(ForwardDir, false);
					current.fence.set(ReverseDir, false);
				}
				
				if (current.observed) { continue; }
				this->plants.push_back(&current);
				current.observed = true;

				for (const Needle& compass : Directions)
				{
					Dir next = curPos + std::get<0>(compass);
					
					if (inBounds(next)) {
						toVisit.emplace_back( next, compass );
					}
				}

			}
		
			fenceCount = 0;
			for (const Plant* p : plants) {
				fenceCount+=p->fence.count();
			}
		}
	
		I sides{};
		void walkFences(Garden& garden, I w, I h) {
			auto inBounds = [w, h](const Dir& pos)->bool {
				if (pos.dx < 0 || pos.dx >= w) { return false; }
				if (pos.dy < 0 || pos.dy >= h) { return false; }
				return true;
				};
			auto markAndWalk = [&garden,inBounds](Plant* p, Dir& d, DLabel mark) {
				while (p != nullptr) {
					p->fenceHandled.set(mark, true);
					Dir next = Dir{ p->x,p->y } + d;
					if (!inBounds(next)) { return; }
					Plant& nextPlant = garden[next.dy][next.dx];
					if (nextPlant.name != p->name
						|| !nextPlant.fence.test(mark)
						) {
						return; 
					}
					p = &nextPlant;
				}
				};

			I wSides{};
			I eSides{};
			I nSides{};
			I sSides{};

			sides = 0;
			for (auto itP = plants.begin(); itP != plants.end(); ++itP) {
				Plant* p = *itP;
				if (p->fence.test(N) && !p->fenceHandled.test(N))
				{
					Dir forward = std::get<0>(Directions[E]);
					Dir backard = std::get<0>(Directions[W]);
					markAndWalk(p, forward, N);
					markAndWalk(p, backard, N);
					nSides++;
					sides++;
				}
				if (p->fence.test(S) && !p->fenceHandled.test(S))
				{
					Dir forward = std::get<0>(Directions[E]);
					Dir backard = std::get<0>(Directions[W]);
					markAndWalk(p, forward, S);
					markAndWalk(p, backard, S);
					sSides++;
					sides++;
				}
				if (p->fence.test(E) && !p->fenceHandled.test(E))
				{
					Dir forward = std::get<0>(Directions[N]);
					Dir backard = std::get<0>(Directions[S]);
					markAndWalk(p, forward, E);
					markAndWalk(p, backard, E);
					eSides++;
					sides++;
				}
				if (p->fence.test(W) && !p->fenceHandled.test(W))
				{
					Dir forward = std::get<0>(Directions[N]);
					Dir backard = std::get<0>(Directions[S]);
					markAndWalk(p, forward, W);
					markAndWalk(p, backard, W);
					wSides++;
					sides++;
				}
			}
			
		}
	};

	I h{};
	I w{};
	Garden garden{};
	Region startCandidates{};

	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		w = line.length();
		Row row; row.resize(w);
		char start{};
		for (size_t x = 0; x < w; ++x) {
			Plant& p = row[x];
			p.x = x;
			p.y = h;
			p.name = line[x];
			if (start != line[x]) {
				startCandidates.plants.push_back(&p);
				start = line[x];
			}
		}
		garden.emplace_back(std::move(row));
		++h;
	}
	std::println(std::cout, "w: {} h: {}", w, h);
	
	std::vector<Region> regions{};
	for (Plant* tst : startCandidates.plants) {
		Plant& plant = *tst;
		if (plant.observed) { continue; }
		Region r{};
		r.name = plant.name;
		r.fill(plant, garden,w,h);
		r.walkFences(garden, w, h);
		regions.emplace_back(std::move(r));
	}

	I price{};
	I sidesPrice{};
	for (const Region& region : regions) {
		//std::println(std::cout, "{:c} : {:>5} x {:>5} : {:>5} sides", region.name, region.plants.size(), region.fenceCount, region.sides);
		price += (region.plants.size()*region.fenceCount);
		sidesPrice += (region.plants.size() * region.sides);
	}
	std::println(std::cout, "{}", price);
	std::println(std::cout, "{}", sidesPrice);
}

