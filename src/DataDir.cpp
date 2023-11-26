#include "DataDir.h"
#include <string>
#include <string_view>
#include <iostream>
#include <fstream>

constexpr std::string_view cpp_template = R"temp(#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <cassert>

void {{func_name}}()
{
	fs::path input(DataDir() / "{{specific_year}}_{{specific_day}}.txt");
	TxtFile txt{ input };

	__int64 count = 0LL;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { continue; }
		++count;
	}
	std::cout << "count: " << count << "\n";
	assert(count == 42);
}

)temp";

std::string createTemplate(std::string_view specific, int day)
{
	using ReplacePattern = std::tuple<std::string, std::string>;
	using ReplacePatterns = std::vector< ReplacePattern >;
	ReplacePatterns patterns{
		{"{{func_name}}", std::format("aoc{:0>4}_{:0>2}", specific, day) },
		{"{{specific_year}}", std::format("{:0>4}", specific)},
		{"{{specific_day}}", std::format("{:0>2}", day) },	
	};
	std::string outstr{ cpp_template };
	for (const auto& [pattern, newValue] : patterns)
	{
		size_t pos = 0;
		while ((pos = outstr.find(pattern,pos)) != std::string::npos)
		{
			outstr.replace(pos, pattern.length(), newValue);
			pos += newValue.length();
		}
	}
	return outstr;
}

void createDataDirIfNotExists(const fs::path& dataDir, std::string_view specific)
{
	if (!fs::exists(dataDir))
	{
		fs::create_directory(dataDir);
		for (int i = 1; i <= 25; ++i)
		{
			std::string txt = std::format("{:0>4}_{:0>2}.txt", specific, i);
			fs::path fn_txt = dataDir / txt;
			if (!fs::exists(fn_txt)) {
				std::ofstream os(fn_txt);
			}
			std::string cpp = std::format("aoc{:0>4}_{:0>2}.cpp", specific, i);
			fs::path fn_cpp = dataDir / cpp;
			if (!fs::exists(fn_cpp)) {
				std::string content = createTemplate(specific, i);
				std::ofstream os(fn_cpp);
				os.write(content.c_str(), content.size());
			}
		}
	}
}

const fs::path& DataDir(std::string_view exe,std::string_view specific)
{
	static fs::path dataDir;
	if (exe.length() > 0)
	{
		fs::path exeFile(exe);
		fs::path exePath( fs::canonical(exeFile).parent_path());
		dataDir = exePath.parent_path() / specific;
		createDataDirIfNotExists(dataDir, specific);
		std::cout << "DataDir: \"" << dataDir.string() << "\"\n";
	} 

	return dataDir;
}