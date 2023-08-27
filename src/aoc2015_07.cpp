#include "DataDir.h"
#include "LineIterator.h"
#include "SplitIterator.h"
#include "TxtFile.h"
#include <numeric>
#include <iostream>
#include <format>
#include <optional>
#include <functional>
#include <map>
#include <cassert>

using Signal = unsigned short;

struct Wire
{
	using WireMap = std::map<std::string, Wire>;
	std::optional<Signal> output;
	std::function<Signal(Wire&, WireMap&)> CalcOutput;
	std::string name;
	Wire* lhs=nullptr;
	std::string opString; //fuer output
	Wire* rhs=nullptr;
	Signal Output(WireMap& wires) {
		if (output) { return output.value(); }
		if (CalcOutput)
		{
			output = CalcOutput(*this, wires);
			return output.value();
		}
		return {};
	}
	bool isCalculated() { return lhs || rhs; }
	void clearOutput() { if (isCalculated()) { output = {}; } }
};

static Wire* getCreateOutput(std::string name, Wire::WireMap& wires)
{
	if (name.length() == 0) { return nullptr; }

	Wire* w= &wires[name];
	if (w->name.length() == 0) { w->name = name; }
	if (atol(name.c_str()) != 0 
		|| name=="0"
		)
	{
		w->output = static_cast<Signal>(std::stoi(name));
	}
	return w;
}

//assignment zu input op input -> output
void addConnection(const std::string_view& assignment, Wire::WireMap& wires)
{
	if (assignment.length() == 0) { return; }
	SplitIterator<' '> begin{ assignment }; //oder direkt ein Reverse_SplitIterator?
	SplitIterator<' '> end{};
	std::vector<std::string_view> items;
	for (; begin != end; ++begin){items.emplace_back(*begin);}
	
	//target,<-,rhs,op,lhs
	Wire* target = nullptr;
	std::string op = "";
	int state = 0;
	for (auto it = items.rbegin(); it != items.rend(); ++it)
	{
		std::string val{ *it };
		if (state == 0) // target
		{
			target = getCreateOutput(val,wires);
			++state;
			continue;
		}
		else if (state == 1) //->
		{
			++state;
			continue;
		}
		else if (state == 2) //rhs
		{
			target->rhs = getCreateOutput(val, wires);
			++state;
		}
		else if( state == 3 ) //op
		{ 
			op = val;
			target->opString = op;
			++state;
		}
		else if (state == 4) //lhs
		{
			target->lhs = getCreateOutput(val, wires);
			break;
		}
	}

	if (op.length() == 0)//assign
	{
		target->CalcOutput = [](Wire& self, Wire::WireMap& wires) {
			return self.rhs->Output(wires);
		};
	}
	else if (op == "NOT")
	{
		target->CalcOutput = [](Wire& self, Wire::WireMap& wires) {
			Signal inv= self.rhs->Output(wires);
			return ~inv;
		};
	}
	else if (op == "AND")
	{
		target->CalcOutput = [](Wire& self, Wire::WireMap& wires) {
			Signal rhs = self.rhs->Output(wires);
			Signal lhs = self.lhs->Output(wires);
			return lhs&rhs;
		};
	}
	else if (op == "OR")
	{
		target->CalcOutput = [](Wire& self, Wire::WireMap& wires) {
			Signal rhs = self.rhs->Output(wires);
			Signal lhs = self.lhs->Output(wires);
			return lhs | rhs;
		};
	}
	else if (op == "LSHIFT")
	{
		target->CalcOutput = [](Wire& self, Wire::WireMap& wires) {
			Signal rhs = self.rhs->Output(wires);
			Signal lhs = self.lhs->Output(wires);
			return lhs << rhs;
		};
	}
	else if (op == "RSHIFT")
	{
		target->CalcOutput = [](Wire& self, Wire::WireMap& wires) {
			Signal rhs = self.rhs->Output(wires);
			Signal lhs = self.lhs->Output(wires);
			return lhs >> rhs;
		};
	}

}

void aoc2015_07()
{
	constexpr Signal maxNum=std::numeric_limits<Signal>::max();
	constexpr Signal minNum=std::numeric_limits<Signal>::min();
	std::cout << std::format("Signals in range [{}..{}]\n", minNum, maxNum);

	TxtFile txt{ DataDir() / "2015_07.txt" };

	
	Wire::WireMap wires;

	for (const auto& line : txt)
	{
		if (line.length() == 0)
		{
			break;
		}
		addConnection(line, wires);
	}

	Wire* a = getCreateOutput("a", wires);
	Signal calculated = a->Output(wires);
	std::cout << std::format("\n{:<5}:{:>6}\n", a->name, calculated);
	
	Signal overrid = *a->output;
	assert(overrid == 3176);
	for (auto& [name, wire] : wires)
	{
		wire.clearOutput();
	}
	
	Wire* b = getCreateOutput("b", wires);
	b->output = overrid;
	Signal afterChange=a->Output(wires);
	std::cout << std::format("Wire {:<5}:{:>6}\n", b->name, *b->output);
	std::cout << std::format("Wire {:<5}:{:>6}\n", a->name, *a->output);
	assert(afterChange == 14710);
}