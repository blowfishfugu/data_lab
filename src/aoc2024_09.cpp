#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <charconv>
#include <iostream>
#include <print>

namespace {
	using I = std::int64_t;
	struct Block {
		int fileID{ -1 };
		int chunkSize{0};
		Block* next{nullptr};
		Block* prev{nullptr};
	};

	inline int to_int(char c) {
		return static_cast<int>(c) - '0';
	}

	struct Disk final{
		Block* root{};
		Block* add_block(Block* cursor, int fileID, int chunkSize) {
			if (cursor && cursor->fileID == fileID) {
				cursor->chunkSize += chunkSize;
				return cursor;
			}
			Block* next = alloc_block(fileID, chunkSize);
			if (!cursor) { root = next; }
			else {
				cursor->next = next;
			}
			next->prev = cursor;
			return next;
		}

		std::vector<Block*> pool;
		Block* alloc_block(int fileID, int chunkSize) {
			Block* b = new Block{ fileID,chunkSize,nullptr,nullptr };
			pool.push_back(b);
			return b;
		}

		bool repartition_block(Block* from, Block* emptyBlock) {
			if (from == nullptr) {
				return false;
			}
			if (from->fileID == -1) { //empty, nothing to move
				return true;
			}

			int initialSize = from->chunkSize;
			while (from->chunkSize > 0) {
				while (emptyBlock->fileID != -1 || emptyBlock->chunkSize==0 
					|| emptyBlock->chunkSize < from->chunkSize //part2 <-fullFit
					) {
					emptyBlock = emptyBlock->next;
					if (emptyBlock == from)
					{
						return false; //reached end
					}
				}



				Block* target = nullptr;
				if (emptyBlock->prev != nullptr && emptyBlock->prev->fileID == from->fileID) {
					target = emptyBlock->prev;
				}
				else {
					target = alloc_block(from->fileID, 0);
					if (emptyBlock->prev != nullptr)
					{
						emptyBlock->prev->next = target;
					}
					target->prev = emptyBlock->prev;
					target->next = emptyBlock;
					emptyBlock->prev = target;
				}

				while (emptyBlock->chunkSize > 0 && from->chunkSize > 0) {
					--emptyBlock->chunkSize;
					--from->chunkSize;
					++target->chunkSize;
				}
			}
			//from is now a emptyBlock
			if (from->chunkSize == 0){
				from->fileID = -1; from->chunkSize = initialSize; 
				return true; //fully moved
			}
			return false;
		}
		void print() const {
			Block* current = root;
			while (current != nullptr) {
				if (current->fileID != -1)
				{
					for (size_t i = 0; i < current->chunkSize; ++i) {
						std::print(std::cout, "{}", current->fileID);
					}
				}
				else
				{
					for (size_t i = 0; i < current->chunkSize; ++i) {
						std::print(std::cout, "{}", '.');
					}
				}
				current = current->next;
			}
			std::print(std::cout, "{}",'\n');
		}

		I checksum() const {
			I sum = 0;
			I index = 0;
			Block* current = root;
			while (current != nullptr) {
				for (size_t i = 0; i < current->chunkSize; ++i) {
					if (current->fileID != -1)
					{
						I prod = index * current->fileID;
						sum += prod;
					}
					++index;
				}
				current = current->next;
			}
			return sum;
		}

		void format() {
			for (Block* b : pool) { delete b; }
		}
		~Disk(){
			this->format();
		}
	};
}

void aoc2024_09()
{
	fs::path input(DataDir() / "2024_09.txt");
	TxtFile txt{ input };

	Disk disk{};
	int fileIDs{}; //linesize/2?
	int fileSum{};
	int freeSum{};
	Block* cursor = nullptr;
	for (const auto& line : txt)
	{
		if (line.length() == 0) { break; }

		bool isEmpty = false;
		for (size_t i = 0; i < line.size(); ++i) {
			int sz = to_int(line[i]);
			if (!isEmpty){
				cursor = disk.add_block(cursor, fileIDs, sz);
				fileIDs++;
				fileSum += sz;
			}
			else {
				cursor = disk.add_block(cursor, -1, sz);
				freeSum += sz;
			}

			isEmpty = !isEmpty;
		}
	}

	//disk.print();

	Block* lastBlock = cursor;
	Block* nextEmpty = disk.root;
	while (lastBlock != disk.root )
	{
		disk.repartition_block(lastBlock, nextEmpty);
		//disk.print();
		if (!lastBlock)
		{ 
			break; 
		}
		lastBlock = lastBlock->prev;
	}

	//disk.print();
	std::println(std::cout, "checksum {}", disk.checksum());
	std::println(std::cout, "fileIDs: {}", fileIDs);
	std::println(std::cout, "fileSum: {}", fileSum);
	std::println(std::cout, "freeSum: {}", freeSum);
	std::println(std::cout, "diskSize: {}", fileSum+freeSum);
	
}

