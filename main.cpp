#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"
#include "src/Days2022.h"

int main(int argc, char** argv)
{
	DataDir(argv[0]);//Set
	aoc2022_01();
	return 0;
}