#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2023.h"

int main(int argc, char** argv)
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0], "2023");//Set

	StopWatch clk;

	//aoc2023_01(); clk.printDelta("Day01"); std::cout << "\n\n";
	//aoc2023_02(); clk.printDelta("Day02"); std::cout << "\n\n";
	//aoc2023_03(); clk.printDelta("Day03"); std::cout << "\n\n";
	//aoc2023_04(); clk.printDelta("Day04"); std::cout << "\n\n";
	//aoc2023_05(); clk.printDelta("Day05"); std::cout << "\n\n";
	//aoc2023_06(); clk.printDelta("Day06"); std::cout << "\n\n";
	//aoc2023_07(); clk.printDelta("Day07"); std::cout << "\n\n";
	//aoc2023_08(); clk.printDelta("Day08"); std::cout << "\n\n";
	//aoc2023_09(); clk.printDelta("Day09"); std::cout << "\n\n";
	//aoc2023_10(); clk.printDelta("Day10"); std::cout << "\n\n"; //<-TODO:make nicer
	//aoc2023_11(); clk.printDelta("Day11"); std::cout << "\n\n";
	aoc2023_12(); clk.printDelta("Day12"); std::cout << "\n\n"; //TODO: skipped in replication
	//aoc2023_13(); clk.printDelta("Day13"); std::cout << "\n\n";

	return 0;
}