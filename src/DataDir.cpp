#include "DataDir.h"
#include <string>
#include <string_view>
#include <iostream>

const fs::path& DataDir(std::string_view exe)
{
	static fs::path dataDir;
	if (exe.length() > 0)
	{
		fs::path exeFile(exe);
		fs::path exePath( fs::canonical(exeFile).parent_path());
		dataDir = exePath.parent_path() / "data";

		if (!fs::exists(dataDir))
		{
			fs::create_directory(dataDir);
		}
		std::cout << "DataDir: \"" << dataDir.string() << "\"\n";
	} 

	return dataDir;
}