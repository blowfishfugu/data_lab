#include "DataDir.h"
#include <iostream>
#include <sstream>
#include <tuple>
#include <array>
#include <vector>
#include <map>
#include <algorithm>
#include <functional>
#include <numeric>
#include <deque>

struct Vec3
{
	__int64 x{};
	__int64 y{};
	__int64 z{};
	friend bool operator <(const Vec3& lhs, const Vec3& rhs) noexcept {
		return std::tie(lhs.x, lhs.y,lhs.z) < std::tie(rhs.x, rhs.y,rhs.z);
	}
	friend bool operator >(const Vec3& lhs, const Vec3& rhs) noexcept {
		return std::tie(lhs.x, lhs.y, lhs.z) > std::tie(rhs.x, rhs.y, rhs.z);
	}
	friend bool operator ==(const Vec3& lhs, const Vec3& rhs)  noexcept {
		return std::tie(lhs.x, lhs.y, lhs.z) == std::tie(rhs.x, rhs.y, rhs.z);
	}
	friend bool operator !=(const Vec3& lhs, const Vec3& rhs)  noexcept {
		return std::tie(lhs.x, lhs.y, lhs.z) != std::tie(rhs.x, rhs.y, rhs.z);
	}
};

Vec3 operator+(const Vec3& s, const Vec3& e)
{
	return { e.x + s.x,e.y + s.y, e.z+s.z };
}

Vec3 operator-(const Vec3& s, const Vec3& e)
{
	return { s.x - e.x,s.y - e.y, s.z - e.z };
}

struct Cube
{
	//Vec3 center{}; //<- reicht map?
	int directNeighbours{};
	int surface{ 6 };
};

struct Steam
{
	int touched{ 0 };
};

Vec3 read(const std::string& str)
{
	Vec3 c;
	std::istringstream in(str);
	std::string v;
	std::getline(in, v, ',');
	c.x = std::stoi(v);
	std::getline(in, v, ',');
	c.y = std::stoi(v);
	std::getline(in, v);
	c.z = std::stoi(v);
	return c;
}

void aoc2022_18()
{
	std::map<Vec3, Cube> cubeMap;
	
	fs::path input(DataDir() / "2022_18.txt");
	std::ifstream inFile(input);
	
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0) { break; }
		cubeMap.emplace(read(line), Cube{});
	}

	std::array<Vec3,6> inspect{
		Vec3{ 1,0,0 },
		{ -1,0,0 },
		{ 0, 1, 0 },
		{ 0, -1, 0 },
		{ 0, 0, 1 },
		{ 0, 0, -1 }
	};
	__int64 iMin = std::numeric_limits<__int64>::min();
	__int64 iMax = std::numeric_limits<__int64>::max();
	
	//BB
	Vec3 pMin{iMax,iMax,iMax};
	Vec3 pMax{iMin,iMin,iMin};

	for (auto&[ pos, cube] : cubeMap)
	{
		pMin.x = std::min(pMin.x, pos.x);
		pMin.y = std::min(pMin.y, pos.y);
		pMin.z = std::min(pMin.z, pos.z);
		pMax.x = std::max(pMax.x, pos.x);
		pMax.y = std::max(pMax.y, pos.y);
		pMax.z = std::max(pMax.z, pos.z);
		
		for (const Vec3& dir : inspect)
		{
			if (cubeMap.find(dir + pos) != cubeMap.end())
			{
				++cube.directNeighbours;
				--cube.surface;
			}
		}
	}

	Vec3 boundingSize = pMax - pMin;

	using cubePair = std::map<Vec3, Cube>::value_type;
	__int64 score = std::accumulate(cubeMap.cbegin(),cubeMap.cend(), 0LL,
		[](__int64 val, const cubePair& rhs)->__int64 { return val+rhs.second.surface; });
	std::cout << score << "\n";//4444

	pMin = pMin + Vec3{-1, -1, -1 };
	pMax = pMax + Vec3{ 1, 1, 1 };

	std::deque<Vec3> toVisit;
	std::map<Vec3, Steam> visited;
	toVisit.push_back(pMin);
	toVisit.push_back(pMax);
	__int64 touchSum = 0;
	while (toVisit.size() > 0)
	{
		Vec3 current = toVisit.front();
		toVisit.pop_front();
		if (visited.find(current) != visited.end())
		{
			continue;
		}

		
		Steam tst;
		for (const Vec3& dir : inspect)
		{
			Vec3 lookAt = dir + current;
			if (cubeMap.find(dir + current) != cubeMap.end())
			{
				tst.touched++;
				touchSum++;
			}
			else
			{
				if (
					(lookAt.x >= pMin.x && lookAt.x <= pMax.x)
					&& (lookAt.y >= pMin.y && lookAt.y <= pMax.y)
					&& (lookAt.z >= pMin.z && lookAt.z <= pMax.z)
					)
				{
					toVisit.emplace_back(lookAt);
				}
			}
		}

		visited.emplace(current, tst);
	}

	//testfall freier würfel, kommt 6 raus?
	//for (auto& [k,s] : visited)
	//{
	//	if (s.touched > 0)
	//	{
	//		std::cout << "Touched " << s.touched << "\tfrom\t" 
	//			<<" " << k.x << " " << k.y << " " << k.z << "\n";
	//	}
	//}

	using steamPair = std::map<Vec3, Steam>::value_type;
	__int64 outer = std::accumulate(visited.cbegin(), visited.cend(), 0LL,
		[](__int64 val, const steamPair& rhs)->__int64 { return val + rhs.second.touched; });
	std::cout << outer << "\n";//2530

}