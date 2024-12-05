#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2024.h"

int main(int argc, char** argv)
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0], "2024");//Set

	StopWatch clk;
    
	//aoc2024_01(); clk.printDelta("Day01");std::cout<<"\n\n";
	//aoc2024_02(); clk.printDelta("Day02");std::cout<<"\n\n";
	//#aoc2015_19(); clk.printDelta("Day19");std::cout<<"\n\n";
	//aoc2024_03(); clk.printDelta("Day03");std::cout<<"\n\n";
	//aoc2024_04(); clk.printDelta("Day04");std::cout<<"\n\n";
	aoc2024_05(); clk.printDelta("Day05");std::cout<<"\n\n";
	//aoc2024_06(); clk.printDelta("Day06");std::cout<<"\n\n";
	//aoc2024_07(); clk.printDelta("Day07");std::cout<<"\n\n";
	//aoc2024_08(); clk.printDelta("Day08");std::cout<<"\n\n";
	//aoc2024_09(); clk.printDelta("Day09");std::cout<<"\n\n";
	//aoc2024_10(); clk.printDelta("Day10");std::cout<<"\n\n";
	//aoc2024_11(); clk.printDelta("Day11");std::cout<<"\n\n";
	//aoc2024_12(); clk.printDelta("Day12");std::cout<<"\n\n";
	//aoc2024_13(); clk.printDelta("Day13");std::cout<<"\n\n";
	//aoc2024_14(); clk.printDelta("Day14");std::cout<<"\n\n";
	//aoc2024_15(); clk.printDelta("Day15");std::cout<<"\n\n";
	//aoc2024_16(); clk.printDelta("Day16");std::cout<<"\n\n";
	//aoc2024_17(); clk.printDelta("Day17");std::cout<<"\n\n";
	//aoc2024_18(); clk.printDelta("Day18");std::cout<<"\n\n";
	//aoc2024_19(); clk.printDelta("Day19");std::cout<<"\n\n";
	//aoc2024_20(); clk.printDelta("Day20");std::cout<<"\n\n";
	//aoc2024_21(); clk.printDelta("Day21");std::cout<<"\n\n";
	//aoc2024_22(); clk.printDelta("Day22");std::cout<<"\n\n";
	//aoc2024_23(); clk.printDelta("Day23");std::cout<<"\n\n";
	//aoc2024_24(); clk.printDelta("Day24");std::cout<<"\n\n";
	//aoc2024_25(); clk.printDelta("Day25");std::cout<<"\n\n";
	return 0;
}