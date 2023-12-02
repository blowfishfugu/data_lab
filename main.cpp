#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2023.h"

int main(int argc, char** argv)
{
	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0], "2023");//Set

	StopWatch clk;
	
	aoc2023_01();clk.printDelta("Day01");std::cout << "\n\n";
	aoc2023_02();clk.printDelta("Day02");std::cout << "\n\n";
	
	return 0;
}