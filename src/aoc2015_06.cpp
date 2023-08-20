#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"

#include <iostream>
#include <format>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>

#include "gdi_window.h"

void aoc2015_06()
{
	TxtFile txt{ DataDir() / "2015_06.txt" };
	AsyncQueue<RectData> inputs;
	gdi_window EyeCandy(inputs,1000,1000);
	EyeCandy.start_window();
	int i = 0;
	for (const auto& line : txt)
	{
		EyeCandy.trigger_update();
		Sleep(1);
		++i;
	}

	EyeCandy.wait_for_close();
}