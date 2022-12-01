#include <filesystem>
#include <string>
#include <string_view>
#include "DataDir.h"


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
	}

	return dataDir;
}