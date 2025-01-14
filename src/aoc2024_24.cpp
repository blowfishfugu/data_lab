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
#include <set>

namespace D24
{
	using Wirestate = std::optional<std::bitset<1>>;
	struct Gate final{
		std::string name{};
		Wirestate output{};
		std::array<Gate*,2> inputs{nullptr,nullptr};
		bool invalid{ false };
		std::function<Wirestate(Gate*, Gate*)> op{};
		void reset(Wirestate newState = {}) {
			output = newState;
		}

		bool addParent(std::set<Gate*>& visited, Gate* root) {
			if (visited.contains(root)) { return true; }
			if (visited.contains(this)) { return false; }
			visited.insert(this);
			bool hasConflict = false;
			if (inputs[0]) { hasConflict|=inputs[0]->addParent(visited,root); }
			if (inputs[1]) { hasConflict|=inputs[1]->addParent(visited,root); }
			return hasConflict;
		}

		bool getParents(std::set<Gate*>& visited) {
			bool hasConflict = false;
			if (inputs[0]) { hasConflict|=inputs[0]->addParent(visited,this); }
			if (inputs[1]) { hasConflict|=inputs[1]->addParent(visited,this); }
			return hasConflict;
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
			if (readState >= 2) { break; }
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

	void resetGates(std::vector<Gate*>& gates) {
		for (Gate* g : gates) {
			if (g->inputs[0] && g->inputs[1]) { //ignore input-values?
				g->output = {};
				g->invalid = false;
			}
		}
	}

	std::vector<Gate*> getGatesOf(std::vector<Gate*>& gates, char filter) {
		std::vector<Gate*> zxx{};
		for (Gate* g : gates) { if (g->name[0] == filter) { zxx.push_back(g); } }
		std::sort(zxx.begin(), zxx.end(), [](Gate* l, Gate* r) { return l->name > r->name; });
		return zxx;
	}

	//returns z,x,y and bool if solvable
	std::tuple<I, I, I,bool> solveTree(std::vector<Gate*>& gates) {
		auto calcFrom = std::partition(gates.begin(), gates.end(), [](Gate* g)->bool { return g->output.has_value() || g->invalid; });
		while (calcFrom != gates.end())
		{
			for (calcFrom; calcFrom != gates.end(); ++calcFrom) {
				Gate* g = *calcFrom;

				std::set<Gate*> visited{};
				if (bool hasConflict = g->getParents(visited); hasConflict) {
					g->invalid = true;
					continue;
				};

				if (g->output.has_value()) { continue; }
				if (g->inputs[0]->output && g->inputs[1]->output) {
					g->output = g->op(g->inputs[0], g->inputs[1]);
				}
			}
			calcFrom = std::partition(gates.begin(), gates.end(), [](Gate* g)->bool { return g->output.has_value() || g->invalid; });
		}

		auto extractValue = [](std::vector<Gate*>& gates, char filter)->std::tuple<I,bool> {
			std::vector<Gate*> zxx=getGatesOf(gates, filter);
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

	void swapOutput(Gate* l, Gate* r) {
		std::swap(l->inputs, r->inputs);
		std::swap(l->op, r->op);
	}

	I testGates(std::vector<Gate*>& gates, int x, int y) {
			resetGates(gates);
			if ((x == 0 || x == 1) && (y == 0 || y == 1))
			{
				for (Gate* g : gates) {
					if (g->name[0] == 'x') { g->output = x; }
					if (g->name[0] == 'y') { g->output = y; }
				}
			}

			auto [result, inX, inY, ok] = solveTree(gates);
			if (ok)
			{
				I expected = inY + inX;
				I diff = expected ^ result;
				std::println(std::cout, "solvable: {}| expected: {}| Z: {} X: {} Y: {}", ok, (result == expected), result, inX, inY);
				std::println(std::cout, "X:  {:045b}\nY:  {:045b}\n---\nZ: {:046b}\nE: {:046b}\n---\nD: {:046b}", inY, inX, result, expected, diff);
				std::println(std::cout);
				return diff;
			}
			return std::numeric_limits<I>::max();
	}

	bool runTest(std::vector<Gate*>& gates, I& d1, I& d2, I& d3, I& d4, I& d5) {
		d1 = testGates(gates, 0, 0);
		d2 = testGates(gates, 0, 1);
		d3 = testGates(gates, 1, 0);
		d4 = testGates(gates, 1, 1);

		//shuffle
		std::vector<Gate*> xN = getGatesOf(gates, 'x');
		std::vector<Gate*> yN = getGatesOf(gates, 'y');
		for (int s = 1, i = 0; i < xN.size(); ++i) {
			xN[i]->reset(s);
			s = (s == 1) ? 0 : 1;
			yN[i]->reset(s);
		}
		d5 = testGates(gates, 2, 2);

		std::println(std::cout, "\n{:046b}\n{:046b}\n{:046b}\n{:046b}\n{:046b}", d1, d2, d3, d4, d5);

		if (!d1 && !d2 && !d3 && !d4 && !d5)
		{
			std::println(std::cout, "found fit");
			return true;
		}
		return false;
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

	//part1, solve without fiddling
	auto [result,inX,inY,ok]=solveTree(gates);
	
	printGates(gates);

	I expected = inY + inX;
	std::println(std::cout, "solvable: {}| expected: {}| Z: {} X: {} Y: {}", ok,(result==expected), result, inX, inY);
	std::println(std::cout, "X: {:046b}\nY: {:046b}\nZ: {:046b}\nE: {:046b}\nD: {:046b}", inY, inX, result, expected, result^expected);
	std::println(std::cout);
	
	//handpicked sus 12
	//{
	//	Gate* z12 = pool.getCreate("z12");
	//	Gate* qdg = pool.getCreate("qdg");
	//	swapOutput(z12, qdg);
	//}
	

	I d1{}, d2{}, d3{}, d4{}, d5{};
	runTest(gates,d1,d2,d3,d4,d5);
	
	//switchable gates>=sus

	std::vector<Gate*> zN = getGatesOf(gates, 'z');
	std::set<Gate*> vPrev{};
	for (auto it = zN.rbegin(); it != zN.rend();++it) {
		std::set<Gate*> parents{};
		Gate* z = *it;
		z->getParents(parents);
		std::print(std::cout, "{} New: ", z->name);
		
		std::set<Gate*> newGates{};
		for (Gate* p : parents) {
			if (!vPrev.contains(p)) { newGates.insert(p); }
		}
		for (Gate* p : newGates) {
			std::print(std::cout, "{},", p->name);
		}
		std::println(std::cout);
		vPrev = parents;
	}

	int sus = -1;
	for (int i = 0; i < zN.size(); ++i) {
		if ((d5 & 0x01) == 0x01) {
			sus = i;
			break;
		}
		d5 >>= 1;
	}

	
	
	
	
}

