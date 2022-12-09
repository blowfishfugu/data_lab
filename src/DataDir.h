#pragma once
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;
const fs::path& DataDir(std::string_view exe="",std::string_view specific="2022");