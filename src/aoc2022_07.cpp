#include "DataDir.h"
#include <iostream>
#include <array>
#include <memory>
#include <vector>
#include <sstream>
#include <string_view>

void aoc2022_07()
{
	fs::path input(DataDir() / "2022_07.txt");

	std::ifstream inFile(input);

	struct FileNode
	{
		__int64 sizeComplete = 0LL;
		__int64 localFileSizes = 0LL;
		__int64 lsCount = 0;
		std::string name;
		FileNode(std::string_view _name, FileNode* _parent)
			:name(_name),parent(_parent)
		{}
		FileNode* parent = nullptr;
		std::vector<FileNode*> childs;
	};
	using NodePtr = std::unique_ptr<FileNode>;
	struct NodePool
	{
		std::vector<NodePtr> pool;
		NodePool()
		{
			pool.reserve(1000LL/4LL); //input 1020 lines cd+ls+cd..+content
		}
		FileNode* addDir(std::string_view name, FileNode* parent)
		{
			pool.emplace_back(std::make_unique<FileNode>(name, parent));
			FileNode* newDir = pool[pool.size() - 1].get();
			return newDir;
		}
	};

	auto getFileInfo = [](std::string& line)->std::tuple<std::string, std::string>
	{
		std::istringstream splitter(line);
		std::string left;
		std::getline(splitter, left, ' ');
		std::string right;
		std::getline(splitter, right);
		return { left,right };
	};

	NodePool pool;
	FileNode* root = pool.addDir("/", nullptr);
	FileNode* current = nullptr;
	constexpr std::string_view cdSignature = "$ cd ";
	constexpr std::string_view lsSignature = "$ ls";
	constexpr std::string_view dirSignature = "dir";
	__int64 score = 0;
	bool in_ls = false;
	for (std::string line; std::getline(inFile, line); ) {
		if (line.length() == 0) { break; }
		if (line.find(cdSignature) == 0)
		{
			in_ls = false;
			line.erase(0, cdSignature.length());
			if (line == "/")
			{
				current = root;
			}
			else if (line == "..")
			{
				if (current->parent != nullptr)
				{
					current = current->parent;
				}
				else
				{
					std::cout << " invalid cd .. on " << current->name << "\n";
					return;
				}
			}
			else
			{
				bool found = false;
				for (FileNode* child : current->childs)
				{
					if (child->name == line)
					{
						current = child;
						found = true;
						break;
					}
				}
				if (!found)
				{
					std::cout << "invalid directoryname: " << line << " expected in " << current->name << "\n";
					return;
				}
			}
		}
		else if (line.find(lsSignature) == 0)
		{
			line.erase(0, lsSignature.length());
			if (current)
			{
				current->lsCount++; //<- immer nur 1 ls pro ordner?
			}
			in_ls = true;
		}
		else if (in_ls)
		{
			if (current)
			{
				std::tuple<std::string, std::string> fi = getFileInfo(line);
				std::string& info = std::get<0>(fi);
				std::string& name = std::get<1>(fi);
				if (info.find(dirSignature) == 0)
				{
					FileNode* dir = pool.addDir(name, current);
					current->childs.push_back(dir);
				}
				else
				{
					__int64 fileSize = std::stoll(info);
					current->localFileSizes += fileSize;

					//update parents
					FileNode* parent = current; //<-current self-complete
					while (parent != nullptr)
					{
						parent->sizeComplete += fileSize;
						parent = parent->parent;
					}
					/*
					FileNode* file = pool.addDir(name, current);
					file->localFileSizes = fileSize;
					current->childs.push_back(file);
					*/
				}
			}
		}
	}
	std::sort(pool.pool.begin(), pool.pool.end(), [](const NodePtr& l, const NodePtr& r) {
		return l->sizeComplete > r->sizeComplete;
		});

	constexpr __int64 totalDiskspace = 70'000'000;
	constexpr __int64 wantedFreespace = 30'000'000;
	__int64 available = totalDiskspace - root->sizeComplete;
	__int64 neededDelta = wantedFreespace - available;
	__int64 suitableDirSize = 0;
	FileNode* suitableDir=nullptr;
	for (auto& node : pool.pool)
	{
		if (node->sizeComplete <= 100'000)
		{
			score += node->sizeComplete;
		}
		if (node->sizeComplete >= neededDelta)
		{
			suitableDir = node.get();
		}
	}

	std::cout << "Sum of 'at most' 100000: " << score << "\n";
	if (suitableDir)
	{
		std::cout << "Smallest deletable: '" << suitableDir->name << "' " << suitableDir->sizeComplete << "\n";
	}
}