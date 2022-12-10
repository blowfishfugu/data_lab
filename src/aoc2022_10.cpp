#include "DataDir.h"
#include <iostream>
#include <array>
#include <vector>
#include <functional>
#include <algorithm>

void aoc2022_10()
{
	fs::path input(DataDir() / "2022_10.txt");
	std::ifstream inFile(input);

	std::vector<std::string> commands;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }
		commands.emplace_back(line);
	}

	struct CPU {
		__int64 cycle = 0;
		__int64 X = 1;
		void reset()
		{
			cycle = 0;
			X = 1;
		}
		std::function<bool(CPU&)> onTick = [](CPU& cpu)->bool {return true; };
		void incCycle() {
			cycle++;
			if (onTick(*this))
			{
				std::cout << cycle << "\t" << X <<"\n";
			}
		}

		void noop() {
			incCycle();
		}
		void addx(__int64 n)
		{
			incCycle();
			incCycle();
			//"After two cycles, the X register is increased"
			X += n;
		}
		void run(const std::vector<std::string>& commands)
		{
			for (const std::string& cmd : commands)
			{
				if (cmd == "noop") { noop(); }
				if (cmd.find("addx ") == 0) {
					__int64 inc = std::stoll(cmd.substr(5));
					addx(inc);
				}
			}
		}
	};

	CPU cpu;
	//Part1, Signalstrength
	cpu.reset();
	__int64 checkInterval = 20;
	__int64 sum = 0;
	cpu.onTick = [&checkInterval,&sum](CPU& c) {
		if (c.cycle == checkInterval)
		{
			__int64 signalStrength = c.cycle*c.X;
			std::cout << signalStrength << "\t<<\t";
			sum += signalStrength;
			checkInterval += 40;
			return true;
		}
		return false;
	};
	cpu.run(commands);

	std::cout << sum << "\n\n";

	//Part2, DrawPixelPos
	cpu.reset();
	__int64 pixelPos = 0;
	cpu.onTick = [&pixelPos](CPU& c){
		constexpr unsigned char filled = 219;
		constexpr unsigned char empty = '.';
		const __int64 sprite0 = c.X - 1;
		const __int64 sprite2 = c.X + 1;
		if (pixelPos >= sprite0 && pixelPos<=sprite2){
			std::cout << filled; 
		}
		else {
			std::cout << empty; 
		}
		pixelPos++;
		if (c.cycle % 40 == 0) {
			std::cout << " < " << c.cycle << "\n";
			pixelPos = 0; 
		}
		return false;
	};

	cpu.run(commands);


	
}