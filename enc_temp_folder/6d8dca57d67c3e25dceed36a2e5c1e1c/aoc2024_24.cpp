#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>
#include <bitset>
#include <functional>
#include <optional>
#include <array>
#include <map>
#include <print>
#include <regex>
#include <deque>

namespace D24
{
	using Wirestate = std::optional<std::bitset<1>>;
	struct Gate final{
		std::string name{};
		Wirestate output{};
		std::array<Gate*,2> inputs{nullptr,nullptr};
		std::function<Wirestate(Gate*, Gate*)> op{};
		void reset(Wirestate newState = {}) {
			output = newState;
		}
	};
	using I = std::int64_t;
	struct ShiftRegister {
		I value{};
		void push(Gate* g) {
			value = value << 1;
			if (g->output) {
				value |= (*g->output)==1?0x1:0x0;
			}
		}
	};

	struct GatePool final{
		std::map<std::string, Gate*> gates{};
		Gate* getCreate(const std::string& name) {
			if (auto it = gates.find(name); it != gates.end()) {
				return it->second;
			}
			Gate* g = new Gate();
			g->name = name;
			auto inserted = gates.emplace(name, g);
			return inserted.first->second;
		}

		~GatePool(){
			for (auto& [_, gate] : gates) { delete gate; gate = nullptr; }
		}
	};

	auto parseInitValue = [](const std::string_view& line)->std::tuple<std::string,std::bitset<1>>{
		static std::regex m(R"ex((.+): (\d+))ex");
		std::string cpy(line.cbegin(), line.cend());
		if (std::smatch sm; std::regex_match(cpy, sm, m)) {
			if (sm.size() == 3) {
				std::string name{ sm[1].str() };
				std::bitset<1> val = sm[2].str()[0]-'0';
				return { name,val };
			}
		}
		std::println(std::cerr, "invalid init-input: '{}'", line);
		return {};
	};

	auto parseOpValue = [](const std::string_view& line)->std::tuple<std::string, std::string, std::string, std::string> {
		static std::regex m(R"ex((.+) (AND|XOR|OR) (.+) -> (.+))ex");
		std::string cpy(line.cbegin(), line.cend());
		if (std::smatch sm; std::regex_match(cpy, sm, m)) {
			if (sm.size() == 5) {
				std::string p1{ sm[1].str() };
				std::string op{ sm[2].str() };
				std::string p2{ sm[3].str() };
				std::string target{ sm[4].str() };
				return { p1,op,p2,target };
			}
		}
		std::println(std::cerr, "invalid op-input: '{}'", line);
		return {};
		};

	void parseInputs(const TxtFile& txt, GatePool& pool) {
		int readState = 0;
		for (const auto& line : txt)
		{
			if (line.length() == 0) { readState++; continue; }
			if (readState == 2) { break; }
			if (readState == 0) {
				auto [name, val] = parseInitValue(line);
				Gate* g = pool.getCreate(name);
				g->reset(val);
				continue;
			}
			if (readState == 1) {
				auto [p1, op, p2, target] = parseOpValue(line);
				Gate* g1 = pool.getCreate(p1);
				Gate* g2 = pool.getCreate(p2);
				Gate* gOut = pool.getCreate(target);
				gOut->inputs = { g1,g2 };
				switch (op[0])
				{
				case('O'): { gOut->op = [](Gate* l, Gate* r) { return *l->output | *r->output; }; }; break;
				case('A'): { gOut->op = [](Gate* l, Gate* r) { return *l->output & *r->output; }; }; break;
				case('X'): { gOut->op = [](Gate* l, Gate* r) { return *l->output ^ *r->output; }; }; break;
				default:
					std::println(std::cerr, "unknown op in: '{}'", line);
					break;
				}
				continue;
			}
		}
	}

	void clearScreen() {
		// CSI[2J clears screen, CSI[H moves the cursor to top-left corner
		std::cout << "\x1B[2J\x1B[H";
	}

	void printGates(const std::vector<Gate*>& gates) {
		clearScreen();
		for (Gate* g : gates) {
			char outSign = '-';
			if (g->output) {
				outSign = (g->output == 1) ? '1' : '0';
			}

			std::println(std::cout, "{} : {:c} <- {} x {}", g->name, outSign,
				g->inputs[0] ? g->inputs[0]->name : "(NUL)",
				g->inputs[1] ? g->inputs[1]->name : "(NUL)"
			);
		}
		std::println(std::cout);
	}

	std::tuple<I, I, I,bool> solveTree(std::vector<Gate*>& gates) {
		auto calcFrom = std::partition(gates.begin(), gates.end(), [](Gate* g)->bool { return g->output.has_value(); });
		while (calcFrom != gates.end())
		{
			for (calcFrom; calcFrom != gates.end(); ++calcFrom) {
				Gate* g = *calcFrom;

				//if (g->dependsOnItself()) break;

				if (g->output.has_value()) { continue; }
				if (g->inputs[0]->output && g->inputs[1]->output) {
					g->output = g->op(g->inputs[0], g->inputs[1]);
				}
			}
			calcFrom = std::partition(gates.begin(), gates.end(), [](Gate* g)->bool { return g->output.has_value(); });
		}

		auto extractValue = [](std::vector<Gate*>& gates, char filter)->std::tuple<I,bool> {
			std::vector<Gate*> zxx{};
			for (Gate* g : gates) { if (g->name[0] == filter) { zxx.push_back(g); } }
			std::sort(zxx.begin(), zxx.end(), [](Gate* l, Gate* r) { return l->name > r->name; });

			ShiftRegister shift;
			for (Gate* g : zxx) {
				if (!g->output){
					return { {},false };
				}
				shift.push(g);
			}
			return { shift.value,true };
		};

		auto [result,resOk] = extractValue(gates, 'z');
		auto [inX, xOk] = extractValue(gates, 'x');
		auto [inY, yOk] = extractValue(gates, 'y');

		return { result, inX, inY, (resOk && xOk && yOk) };
	}

	void swapWires(Gate* l, Gate* r) {
		std::swap(l->inputs, r->inputs);
	}
}

void aoc2024_24()
{
	using namespace D24;
	fs::path input(DataDir() / "2024_24.txt");
	TxtFile txt{ input };
	GatePool pool{};
	parseInputs(txt,pool);

	std::vector<Gate*> gates{};
	gates.reserve(pool.gates.size());
	for (auto [_, gate] : pool.gates) { gates.push_back(gate); }

	printGates(gates);
	
	auto [result,inX,inY,ok]=solveTree(gates);
	
	printGates(gates);

	std::println(std::cout, "{}|{}| Z: {} Y: {} X: {}", ok,(result==inY+inX), result, inY, inX);
}

