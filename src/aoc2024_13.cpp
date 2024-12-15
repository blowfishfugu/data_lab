#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include "InputConversions.h"
#include <iostream>
#include <cassert>
#include <regex>
#include <print>

namespace D13{
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

	struct Button {
		V vel{};
		I cost{};
	};
	struct Machine {
		Button a{};
		Button b{};
		V t{};
		bool solved{ false };
	};
	auto toButton = [](const std::string_view& line, I cost)->Button {
		static std::regex m(R"ex(.*: X\+([0-9]+), Y\+([0-9]+))ex");
		std::string cpy(line.cbegin(), line.cend());
		if (std::smatch sm; std::regex_match(cpy, sm, m)) {
			if (sm.size() == 3) {
				I x = toInt<I>(sm[1].str());
				I y = toInt<I>(sm[2].str());
				return Button{ V{x,y},cost };
			}
		}
		return {};
		};
	auto toPrize = [](const std::string_view& line)->V {
		static std::regex m(R"ex(.*: X=([0-9]+), Y=([0-9]+))ex");
		std::string cpy(line.cbegin(), line.cend());
		if (std::smatch sm; std::regex_match(cpy, sm, m)) {
			if (sm.size() == 3) {
				I x = toInt<I>(sm[1].str());
				I y = toInt<I>(sm[2].str());
				return {x,y};
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
		const V& target = m.t;
		V apos{};
		for (I pushA = 0; pushA <= 100; ++pushA)
		{
			apos = m.a.vel*pushA;
			for (I pushB = 0; pushB <= 100; ++pushB) {
				V combopos= apos + m.b.vel*pushB;
				if (combopos.x >target.x) { continue; }
				if (combopos.y >target.y) { continue; }
				if (combopos.x ==target.x && combopos.y ==target.y)
				{
					m.solved = true;
					return (pushA * m.a.cost + pushB * m.b.cost);
				}
			}
		}
		return{};
	}

	I solve2(Machine& m) {
		//first calibrate by steeepness,
		// walk each near target (order of "bridges" does not matter),
		// then check each if additional direction hits target
		for (I pushA = 0; pushA <= 100; ++pushA)
		{
			for (I pushB = 0; pushB <= 100; ++pushB) {
				
			}
		}
		return{};
	}
}

void aoc2024_13()
{
	using I = D13::I;
	using Machine = D13::Machine;

	fs::path input(DataDir() / "2024_13.txt");
	TxtFile txt{ input };
	std::vector<Machine> machines=D13::parseMachines(txt);
	
	I sumOfCheapest1{};
	for (Machine& m : machines) {
		I cheapest = solve1(m);
		std::println(std::cout, "{:>10}", cheapest);
		sumOfCheapest1 += cheapest;
	}
	std::println(std::cout, "{}", sumOfCheapest1);
}

