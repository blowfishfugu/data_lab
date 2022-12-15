#include "DataDir.h"
#include <iostream>
#include <array>
#include <map>
#include <vector>
#include <set>
#include <functional>
#include <algorithm>
#include <numeric>
#include <optional>
#include <regex>
#include <execution>
#include <ppl.h>

namespace
{

	struct Vec
	{
		__int64 x = 0LL;
		__int64 y = 0LL;
		friend bool operator <(const Vec& lhs, const Vec& rhs) noexcept {
			return std::tie(lhs.x, lhs.y) < std::tie(rhs.x, rhs.y);
		}
		friend bool operator >(const Vec& lhs, const Vec& rhs) noexcept {
			return std::tie(lhs.x, lhs.y) > std::tie(rhs.x, rhs.y);
		}
		friend bool operator ==(const Vec& lhs, const Vec& rhs)  noexcept {
			return std::tie(lhs.x, lhs.y) == std::tie(rhs.x, rhs.y);
		}
		friend bool operator !=(const Vec& lhs, const Vec& rhs)  noexcept {
			return std::tie(lhs.x, lhs.y) != std::tie(rhs.x, rhs.y);
		}

		__int64 manhattan()
		{
			return std::abs(x) + std::abs(y);
		}
	};

	Vec operator-(const Vec& s, const Vec& e)
	{
		return { e.x - s.x,e.y - s.y };
	}
}
	struct Bounds
	{
		__int64 l = std::numeric_limits<__int64>::max();
		__int64 r = std::numeric_limits<__int64>::min();
		__int64 t = std::numeric_limits<__int64>::max();
		__int64 b = std::numeric_limits<__int64>::min();
		void updateX(const __int64 x)
		{
			l = std::min(l, x);
			r = std::max(r, x);
		}
		void updateY(const __int64 y)
		{
			t = std::min(t, y);
			b = std::max(b, y);
		}
		void update(const Vec& p)
		{
			updateX(p.x);
			updateY(p.y);
		}
		void update(const Bounds& b)
		{
			updateX(b.l);
			updateX(b.r);
			updateY(b.t);
			updateY(b.b);
		}
	};

	struct Coverage
	{
		__int64 left = 1;
		__int64 right = -1;
	};

	struct Sensor
	{
		Vec pos;
		Vec closest;
		__int64 maxDistance = 0LL;
		Sensor(const Vec& pos, const Vec& closest)
			:pos(pos), closest(closest)
		{
			maxDistance = (closest - pos).manhattan();
			bounds.updateX(pos.x - maxDistance);
			bounds.updateX(pos.x + maxDistance);
			bounds.updateY(pos.y - maxDistance);
			bounds.updateY(pos.y + maxDistance);
		}
		Bounds bounds;
		Coverage getCoverageY(__int64 y, const __int64 capMin, const __int64 capMax) const
		{
			__int64 dist = std::abs(y - pos.y);
			__int64 remain = maxDistance - dist;
			if (remain >= 0)
			{
				Coverage c{
				 pos.x - remain,
				 pos.x + remain
				};
				if (c.left < capMin) { c.left = capMin; }
				if (c.right > capMax) { c.right = capMax; }
				return c;
			}
			return {1,-1};
		}
	};

using Beacons = std::set<Vec>;
using Sensors = std::vector<Sensor>;

void addSensor(Sensors& sensors, std::string& line)
{
	Vec pos;
	Vec closest;
	const std::regex sensor_regex(".*x=(.\\d*).*y=(.\\d*).*x=(.\\d*).*y=(.\\d*)");
	std::smatch matches;
	if (std::regex_match(line, matches, sensor_regex)) {
			for (size_t i = 1; i < matches.size(); ++i) {
				std::ssub_match sub_match = matches[i];
				__int64 val=std::stoll( sub_match.str() );
				switch (i)
				{
				case(1): pos.x = val; break;
				case(2): pos.y = val; break;
				case(3): closest.x = val; break;
				case(4): closest.y = val; break;
				default:
					break;
				}
			}
		}
	
	sensors.emplace_back(pos, closest);
}

__int64 countCoverages(std::vector<Coverage>& coverages, __int64& singleGap)
{
	__int64 c = 0LL;
	std::sort(coverages.begin(), coverages.end(),
		[](Coverage& lhs, Coverage& rhs) {
			if (lhs.left == rhs.left)
			{
				return lhs.right < rhs.right;
			}
			return lhs.left < rhs.left;
		}
	);
	std::vector<Coverage> joined;
	for (size_t i = 1; i < coverages.size(); ++i)
	{
		Coverage& prev = coverages[i - 1];
		Coverage& current = coverages[i];
		if (current.left <= prev.right)
		{
			current.left = prev.left;
			if (prev.right > current.right)
			{
				current.right = prev.right;
			}
			if (i == coverages.size() - 1)
			{
				joined.push_back(current);
			}
		}
		else
		{
			joined.push_back(prev);
			if ((current.left - prev.right) == 2)
			{
				std::cout << "Gap at: " << prev.right << " to " << current.left << "\n";
				singleGap = current.left - 1LL;
			}
		}
	}

	for (Coverage& j : joined)
	{
		c += ((j.right - j.left)+1LL);
	}
	return c;
}


void aoc2022_15()
{
	fs::path input(DataDir() / "2022_15.txt");
	std::ifstream inFile(input);

	Sensors sensors;
	Beacons beacons;
	Bounds globalbounds;

	__int64 score = 0;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }
		addSensor(sensors, line);
	}

	for (const Sensor& sensor : sensors)
	{
		globalbounds.update(sensor.bounds);
		beacons.insert(sensor.closest);
	}

	//__int64 y = 2000000;
	const __int64 capMax = 4'000'000;
	
	Concurrency::cancellation_token_source cts;
	Concurrency::run_with_cancellation_token(
		[&](){
			Concurrency::parallel_for(0LL, capMax, [&](__int64 y)
				//std::for_each(std::execution::par, 0LL, capMax, [&](__int64 y)
				//for (__int64 y = 0; y <= capMax; ++y)
				{

					__int64 gap = -1LL;
					std::vector<Coverage> coverages;
					for (const Sensor& sensor : sensors)
					{
						auto cover = sensor.getCoverageY(y, 0, capMax);
						if (cover.right - cover.left > 0)
						{
							coverages.emplace_back(cover);
						}
					}

					__int64 fullCover = countCoverages(coverages, gap);
					if (gap != -1LL)
					{
						std::cout << gap << " " << y << "=> " << gap * 4'000'000 + y << "\n";
						cts.cancel();
						return;
					}

					__int64 beaconCount = std::count_if(beacons.cbegin(), beacons.cend(),
						[y](const Vec& b) { return b.y == y; }
					);
					__int64 rowCover = fullCover - beaconCount;
					//std::cout << y << " rowCover: " << rowCover << "\n";

				}
			);
		}, cts.get_token());
	std::cout << score << "\n";

}