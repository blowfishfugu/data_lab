#include <Windows.h>

#include "src/StopWatch.h"
#include "src/DataDir.h"

extern void join_routine(std::string_view ims, std::string_view paypal);

int main(int argc, char** argv)
{
#ifdef UNIT_TEST
	extern void IteratorCheck();
	IteratorCheck();
#endif

	std::cout << "argv0 = \"" << argv[0] << "\"\n";
	DataDir(argv[0], "paypal");//Set

	StopWatch clk;
	join_routine("ims.csv","paypal.csv");
	clk.printDelta("join_routine");
	
	std::cout << "\n\n";
	return 0;
}