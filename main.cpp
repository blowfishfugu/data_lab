#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2015.h"

int main(int argc, char** argv)
{
	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0], "2015");//Set

	StopWatch clk;

	//aoc2015_01();clk.printDelta("Day01");std::cout << "\n\n";
	//aoc2015_02();clk.printDelta("Day02");std::cout << "\n\n";
	//aoc2015_03();clk.printDelta("Day03");std::cout << "\n\n";
	//aoc2015_04();clk.printDelta("Day04");std::cout << "\n\n";
	aoc2015_05();clk.printDelta("Day05");std::cout << "\n\n";
	return 0;
}