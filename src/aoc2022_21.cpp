#include "DataDir.h"
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <functional>
#include <regex>
#include <set>
	struct Op
	{
		using Map = std::map<std::string, Op>;
		std::string Name; //->wär auch in map möglich..
		__int64 result=0;
		__int64 expect = 0;
		std::string opChoose;
		Op* parent = nullptr;
		void setExpect(Map& m, std::set<Op*>& evolvable)
		{
			if (!parent) { return; }
			Op* other = nullptr;
			Op* self = nullptr;
			bool selfLeft = true;
			if (parent->lhs == Name)
			{
				other= &m[parent->rhs];
				self = &m[parent->lhs];
			}
			else
			{
				other = &m[parent->lhs];
				self = &m[parent->rhs];
				selfLeft = false;
			}

			if (parent->opChoose == "=")
			{
				self->expect = other->result;
				self->result = other->result;
			}
			else if (parent->opChoose == "+")
			{
				//r=p+s -> r-s=p | r-p=s
				self->expect = parent->result - other->result;
				self->result = self->expect;
			}
			else if (parent->opChoose == "-")
			{
				//p= l - r -> l = p+r | r = l-p
				//who's minuend
				if (selfLeft)
				{
					self->expect = parent->result + other->result;
				}
				else
				{
					self->expect = other->result - parent->result;
				}
				self->result = self->expect;
			}
			else if (parent->opChoose == "*")
			{
				self->expect = parent->result / other->result;
				self->result = self->expect;
			}
			else if (parent->opChoose == "/")
			{
				//p= l / r -> l = p*r | r = l / p
				//who's divisor!
				if (selfLeft)
				{
					self->expect = parent->result*other->result;
				}
				else
				{
					self->expect = other->result / parent->result;
				}
				self->result = self->expect;
			}


			//self fulfilled
			//pass on to children
			Op* fleft = &m[lhs];
			Op* fright = &m[rhs];
			if (evolvable.find(fleft) != evolvable.end())
			{
				fleft->setExpect(m, evolvable);
			}
			else if (evolvable.find(fright) != evolvable.end())
			{
				fright->setExpect(m, evolvable);
			}
		}
		std::function<__int64(Op* parent, Op *const, Op *const, Map& m)> Func;
		std::string lhs; //<- ungelenk, Op* spart die map, aber man muss 2stufig einlesen
		std::string rhs;
		__int64 Calc(Op* parent, Map & m) {
			this->parent = parent;
			if (!Func) { return result; }
			auto pLhs= m.find(lhs);
			auto pRhs= m.find(rhs);
			
			Op * l = &(pLhs->second);
			Op * r = &(pRhs->second);
			if (!l || !r) { return result; }
			result = Func(this, l,r, m);
			Func = nullptr;
			return result;
		};

		Op* findAncestorsTo(Op* root, std::set<Op*>& line)
		{
			Op* current = this;
			while (current != root)
			{
				line.insert(current);
				if (current->parent == root)
				{
					break;
				}
				current = current->parent;
			}
			return current;
		}
	};

	static auto Add = [](Op* parent, Op *const lhs, Op *const rhs, Op::Map& m)->__int64
	{
		return lhs->Calc(parent,m) + rhs->Calc(parent, m);
	};
	static auto Sub = [](Op* parent, Op *const lhs, Op *const rhs, Op::Map& m)->__int64
	{
		return lhs->Calc(parent, m) - rhs->Calc(parent, m);
	};
	static auto Mul = [](Op* parent, Op *const lhs, Op *const rhs, Op::Map& m)->__int64
	{
		return lhs->Calc(parent, m) * rhs->Calc(parent, m);
	};
	static auto Div = [](Op* parent, Op *const lhs, Op *const rhs, Op::Map& m)->__int64
	{
		return lhs->Calc(parent, m) / rhs->Calc(parent, m);
	};

	auto buildOp(const std::string& line)
	{
		Op op;
		const std::regex combi_regex("(\\D*): (\\D*) (\\D*) (\\D*)");
		const std::regex literal_regex("(\\D*): (\\d*)");

		std::smatch matches;
		if (std::regex_match(line, matches, combi_regex)) {
			for (size_t i = 1; i < matches.size(); ++i) {
				std::ssub_match sub_match = matches[i];
				switch (i)
				{
				case(1): op.Name = sub_match.str(); break;
				case(2): op.lhs = sub_match.str(); break;
				case(3): {
					op.opChoose = sub_match.str();
					if (op.opChoose == "+") { op.Func = Add; }
					else if (op.opChoose == "-") { op.Func = Sub; }
					else if (op.opChoose == "*") { op.Func = Mul; }
					else if (op.opChoose == "/") { op.Func = Div; }
				}
					break;
				case(4): op.rhs = sub_match.str(); break;
				default:
					break;
				}
			}
		}
		else if (std::regex_match(line, matches, literal_regex))
		{
			for (size_t i = 1; i < matches.size(); ++i) {
				std::ssub_match sub_match = matches[i];
				switch (i)
				{
				case(1): op.Name = sub_match.str(); break;
				case(2): op.result = std::stoll(sub_match.str()); break;
				default:
					break;
				}
			}
		}
		return op;
	}

void aoc2022_21()
{
	Op::Map ops;
	fs::path input(DataDir() / "2022_21.txt");
	std::ifstream inFile(input);
	for (std::string line; std::getline(inFile, line);)
	{
		if (line.length() == 0) { break; }
		Op o = buildOp(line);
		ops.emplace(o.Name, o);
	}

	Op& root = ops["root"];
	__int64 res = root.Calc(nullptr, ops);

	Op& human = ops["humn"];
	std::set<Op*> expectors;
	Op* current = &human;
	Op* grandad = current->findAncestorsTo(&root, expectors);
	grandad->setExpect(ops, expectors);

	__int64 score = human.result;
	std::cout << score << "\n";
}