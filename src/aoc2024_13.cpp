#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <regex>
#include <print>

namespace {
	using I = std::int64_t;
	struct Button {
		I dx{};
		I dy{};
		I cost{};
		I pressed{};
	};
	struct Machine {
		Button a{};
		Button b{};
		Button t{};
		bool solved{ false };
	};
	auto toButton = [](const std::string_view& line, I cost)->Button {
		static std::regex m(R"ex(.*: X\+([0-9]+), Y\+([0-9]+))ex");
		std::string cpy(line.cbegin(), line.cend());
		if (std::smatch sm; std::regex_match(cpy, sm, m)) {
			if (sm.size() == 3) {
				I x = toInt<I>(sm[1].str());
				I y = toInt<I>(sm[2].str());
				return Button{ x,y,cost };
			}
		}
		return {};
		};
	auto toPrize = [](const std::string_view& line)->Button {
		static std::regex m(R"ex(.*: X=([0-9]+), Y=([0-9]+))ex");
		std::string cpy(line.cbegin(), line.cend());
		if (std::smatch sm; std::regex_match(cpy, sm, m)) {
			if (sm.size() == 3) {
				I x = toInt<I>(sm[1].str());
				I y = toInt<I>(sm[2].str());
				return Button{ x,y,1 };
			}
		}
		return {};
		};

	std::vector<Machine> parseMachines(const TxtFile& txt) {
		std::vector<Machine> machines{};
		enum state { undef, A, B, X };
		state s = undef;
		I current = -1;
		for (const auto& line : txt)
		{
			if (line.find("---") == 0) {
				break;
			}
			if (s == undef) {
				current = -1;
				if (line.find("Button A: ") == 0) {
					s = A;
					Machine m{};
					m.a = toButton(line, 3);
					machines.emplace_back(m);
					current = machines.size() - 1;
				}
			}
			else if (s == A) {
				if (line.find("Button B: ") == 0) {
					s = X;
					Machine& m = machines[current];
					m.b = toButton(line, 1);
				}
			}
			else if (s == X)
			{
				if (line.find("Prize: ") == 0) {
					s = undef;
					Machine& m = machines[current];
					m.t = toPrize(line);
				}
			}
		}
		return machines;
	}

	I solve1(Machine& m) {
		for (I pushA = 0; pushA <= 100; ++pushA)
		{
			for (I pushB = 0; pushB <= 100; ++pushB) {
				I sum_dx = m.a.dx * pushA + m.b.dx * pushB;
				I sum_dy = m.a.dy * pushA + m.b.dy * pushB;
				if (sum_dx > m.t.dx) { continue; }
				if (sum_dy > m.t.dy) { continue; }
				if (sum_dx == m.t.dx && sum_dy == m.t.dy)
				{
					m.solved = true;
					return (pushA * m.a.cost + pushB * m.b.cost);
				}
			}
		}
		return{};
	}
}

void aoc2024_13()
{
	fs::path input(DataDir() / "2024_13.txt");
	TxtFile txt{ input };
	std::vector<Machine> machines=parseMachines(txt);
	
	I sumOfCheapest1{};
	for (Machine& m : machines) {
		I cheapest = solve1(m);
		std::println(std::cout, "{:>10}", cheapest);
		sumOfCheapest1 += cheapest;
	}
	std::println(std::cout, "{}", sumOfCheapest1);
}

