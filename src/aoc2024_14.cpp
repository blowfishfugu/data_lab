#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <array>
#include <regex>
#include <map>
#include <numeric>
#include <algorithm>
#include <print>

namespace {
	using I = std::int64_t;
	struct V {
		I x{};
		I y{};
		V operator-(V o) const noexcept { return { x - o.x,y - o.y }; }
		V operator+(V o) const noexcept { return { x + o.x,y + o.y }; }
		V& operator+=(V o) noexcept { x += o.x; y += o.y; return *this; };
		auto operator<=>(const V& r) const = default;
	};
	

	struct Robot {
		constexpr static const I width{ 101 };
		constexpr static const I height{ 103 };
		V pos{};
		V vel{};
	};

	auto toRobot = [](const std::string_view& line)->Robot {
		static std::regex m(R"ex(p=([-\d|\d]+),([-\d|\d]+) v=([-\d|\d]+),([-\d|\d]+))ex");
		std::string cpy(line.cbegin(), line.cend());
		if (std::smatch sm; std::regex_match(cpy, sm, m)) {
			if (sm.size() == 5) {
				I x = toInt<I>(sm[1].str());
				I y = toInt<I>(sm[2].str());
				I dx = toInt<I>(sm[3].str());
				I dy = toInt<I>(sm[4].str());
				return Robot{ V{x,y},V{dx,dy} };
			}
		}
		return {};
		};

	void updateRobot(Robot& r) {
		r.pos += r.vel;
	}
	void updatePhysics(Robot& r) {
		if( r.pos.x<0){ r.pos.x += Robot::width; }
		if (r.pos.x >= Robot::width) { r.pos.x -= Robot::width; }
		
		if (r.pos.y < 0) { r.pos.y += Robot::height; }
		if (r.pos.y >= Robot::height) { r.pos.y -= Robot::height; }
	}
}

void aoc2024_14()
{
	fs::path input(DataDir() / "2024_14.txt");
	TxtFile txt{ input };

	std::vector<Robot> robots;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }
		robots.emplace_back(toRobot(line));
	}
	
	std::string view;
	view.resize((Robot::width+1)* Robot::height, ' ');
	constexpr I stride = Robot::width + 1;
	const I fx = std::midpoint(0LL, Robot::width);
	const I fy = std::midpoint(0LL, Robot::height);
	std::array<I, 4> countPerQuadrant{};
	for (I sec = 1; sec < 10000; ++sec) {
		for (Robot& r : robots) {
			updateRobot(r);
			updatePhysics(r);
		}
		

		if( (((sec-12)% Robot::height) ==0)
			&& (((sec-35)% Robot::width)==0)
			)
		{
			std::fill(view.begin(), view.end(), ' ');
			for (I wy = 0; wy < Robot::height; ++wy) {
				view[Robot::width + wy * stride] = '\n';
			}
			for (Robot& r : robots) {
				view[r.pos.x + r.pos.y * stride] = '#';
			}
			std::print(std::cout, "\n\n\n{}\n\n\n", view);
			std::println(std::cout, "{}", sec);
		}

		if (sec == 100) {
			countPerQuadrant.fill(0LL);
			for (Robot& r : robots) {
				if (r.pos.x < fx && r.pos.y < fy) {
					countPerQuadrant[0]++;
				}
				else if (r.pos.x > fx && r.pos.y < fy) {
					countPerQuadrant[1]++;
				}
				else if (r.pos.x > fx && r.pos.y > fy) {
					countPerQuadrant[2]++;
				}
				else if (r.pos.x < fx && r.pos.y > fy) {
					countPerQuadrant[3]++;
				}
			}
			I checksum = std::accumulate(countPerQuadrant.cbegin(), countPerQuadrant.cend(), 1LL
				, [](I cnt, I val) { return cnt * val; }
			);
			std::println(std::cout, "Product of Quadrantsums {}", checksum);
		}
	}


}

