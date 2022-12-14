#include "DataDir.h"
#include <iostream>
#include <array>
#include <vector>
#include <deque>
#include <set>

	struct cell
	{
		std::tuple<size_t, size_t> coords{ 0,0 };
		int height{ std::numeric_limits<int>::max() };
		int minimumCost{ std::numeric_limits<int>::max() };
		cell* lowest{ nullptr }; //0 left, 1 right, 2 bottom, 3 top
		std::array<cell*, 4> adj{};
	};

	struct Cave
	{
		size_t w{ 0 };
		size_t h{ 0 };
		Cave(size_t w, size_t h)
			: cells(w*h), w(w), h(h)
		{

		}
		std::vector<cell> cells;

		void init(size_t x, size_t y, int height)
		{
			size_t pos = y * w + x;
			cell& c = cells[pos];
			c.height = height;
			c.coords = { y,x };
			c.adj[0] = (x != 0) ? &cells[pos - 1] : nullptr;
			c.adj[1] = (x != (w - 1)) ? &cells[pos + 1] : nullptr;
			c.adj[2] = (y != (h - 1)) ? &cells[pos + w] : nullptr;
			c.adj[3] = (y != 0) ? &cells[pos - w] : nullptr;
		}
		void reset()
		{
			for (cell& c : cells)
			{
				c.minimumCost={ std::numeric_limits<int>::max() };
				c.lowest = nullptr;
			}
		}

		__int64 updateNeighbors(cell* from,cell* target) const
		{
			//Dijkstra
			std::deque<cell*> next;
			from->minimumCost = 0;
			next.push_back(from);
			while (next.size() != 0)
			{
				cell* current = next.front();
				next.pop_front();

				for (int i = 0; i < current->adj.size(); ++i)
				{
					cell* n = current->adj[i];
					if (!n) { continue; }
					//falle: adj[i]=nullptr ->nein, falls man über anderen pfad hier rein kommt, weiter besuchen
					if ( n->height-current->height <= 1)
					{
						int costToVisit = current->minimumCost + 1; //step;// n->entryCost;
						if (costToVisit < n->minimumCost)
						{
							n->minimumCost = costToVisit;
							if (n->lowest != current)
							{
								n->lowest = current;
								if (n == target)
								{
									return n->minimumCost;
								}
							}
							next.push_back(n);
						}
					}
				}
			}
			return -1;
		}
#define SLOWMODE 1
		__int64 printPath(cell* target)
		{
			std::set<std::tuple<size_t, size_t>> cellsinpath;

			cell* current = target;//&cells[cells.size() - 1]; //unten-rechts nach 0,0
			std::vector<cell*> path;
			path.push_back(current);
			cellsinpath.insert(current->coords);
			while (current->lowest)
			{
				current = current->lowest;
				path.push_back(current);
				cellsinpath.insert(current->coords);
			}
			std::sort(begin(path), end(path), [](cell* l, cell* r) {
				return l->coords < r->coords;
				});
			/*for (cell* c : path)
			{
#if SLOWMODE
				std::cout << std::get<0>(c->coords) << " , " << std::get<1>(c->coords);
				std::cout << " entry: " << c->height << " mini: " << c->minimumCost << "\n";
#endif
			}*/
#if SLOWMODE
			for (size_t y = 0; y < h; ++y) {
				std::string line;
				for (size_t x = 0; x < w; ++x) {
					size_t pos = y * w + x;
					cell& c = cells[pos];
					if (cellsinpath.find(c.coords) != cellsinpath.end())
					{
						line.push_back('#');
					}
					else
					{
						line.push_back('.'); // c.entryCost;
					}
				}
				std::cout << line.c_str() << "\n";
			}
#endif

			return cellsinpath.size()-1; //1=self
		}
	};

	
	//readPart2 = part1*5
	



void aoc2022_12()
{
	fs::path input(DataDir() / "2022_12.txt");
	std::ifstream inFile(input);
	__int64 score = 0;
	
	std::vector<std::string> data;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }
		data.emplace_back(line);
	}
	if (data.size() == 0) { return; }
	const size_t w = data.front().length();
	const size_t h = data.size();
	Cave* cave = new Cave(w, h);
	size_t starty = 0;
	size_t startx = 0;
	size_t targety = 0;
	size_t targetx = 0;
	std::vector<cell*> aCells;
	for (size_t y = 0; y < h; ++y)
	{
		for (size_t x = 0; x < w; ++x)
		{
			int height = data[y][x];
			if (data[y][x] == 'S')
			{
				height = 'a'; 
				starty = y;
				startx = x;
			}
			if (data[y][x] == 'E')
			{
				height = 'z'; 
				targety = y;
				targetx = x;
			}
			cave->init(x, y, height);
			if (height == 'a')
			{
				aCells.push_back(&cave->cells[y*w + x]);
			}
		}
	}

	__int64 steps = 0;
	cell* start = &cave->cells[starty*w+startx];
	cell* target = &cave->cells[targety*w + targetx];
	steps= cave->updateNeighbors(start, target);
	steps = cave->printPath(target);
	__int64 fewest = std::numeric_limits<__int64>::max();
	
	for (cell* acell : aCells)
	{
		cave->reset();
		__int64 targetSteps = cave->updateNeighbors(acell, target);
		if (targetSteps != -1 && targetSteps < fewest)
		{
			fewest = targetSteps;
		}
	}
	std::cout <<"S to E "<< steps << "\n"; //440
	std::cout <<"smallest a to E "<< fewest << "\n"; //439
	if (cave) { delete cave; cave = nullptr; }

}