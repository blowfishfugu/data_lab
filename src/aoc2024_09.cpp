#include "DataDir.h"
#include "LineIterator.h"
#include "TxtFile.h"
#include <iostream>
#include <print>

namespace {
	using I = std::int64_t;
	struct Block final{
		int fileID{ -1 };
		int chunkSize{0};
		Block* next{nullptr};
		Block* prev{nullptr};
	};

	inline int to_int(char c) {
		return static_cast<int>(c) - '0';
	}

	auto isNoEmptyBlock = [](Block* emptyCandidate, Block* toFit)->bool {
		return (emptyCandidate->fileID != -1 || emptyCandidate->chunkSize == 0);
		};

	auto isNoEmptyFittingBlock = [](Block* emptyCandidate, Block* toFit)->bool {
		return (emptyCandidate->fileID != -1 || emptyCandidate->chunkSize == 0
			|| emptyCandidate->chunkSize < toFit->chunkSize //part2 <-fullFit);
			);
		};

	struct Disk final{
		Block* root{};
		Block* last{};
		Block* add_block(Block* cursor, int fileID, int chunkSize) {
			if (cursor && cursor->fileID == fileID) {
				cursor->chunkSize += chunkSize;
				return cursor;
			}
			Block* next = new Block(fileID, chunkSize, nullptr, cursor);
			if (!cursor) { root = next; }
			else {
				cursor->next = next;
			}
			return next;
		}

		template<typename FitCondition>
		bool repartition_block(Block* from, Block* emptyBlock, FitCondition isNoEmpty) {
			if (from == nullptr) {
				return false;
			}
			if (from->fileID == -1) { //empty, nothing to move
				return true;
			}

			int initialSize = from->chunkSize;
			while (from->chunkSize > 0) {
				while (isNoEmpty(emptyBlock,from) ) {
					emptyBlock = emptyBlock->next;
					if (emptyBlock == from){
						return false; //reached end
					}
				}

				Block* target = nullptr;
				if (emptyBlock->prev != nullptr && emptyBlock->prev->fileID == from->fileID) {
					target = emptyBlock->prev;
				}
				else {
					target = new Block(from->fileID, 0, emptyBlock, emptyBlock->prev);
					if (emptyBlock->prev != nullptr)
					{
						emptyBlock->prev->next = target;
					}
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
				if (current->fileID != -1) {
					for (size_t i = 0; i < current->chunkSize; ++i) {
						std::print(std::cout, "{}", current->fileID);
					}
				}
				else {
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
					if (current->fileID != -1) {
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
			while (root) {
				Block* current = root;
				root = root->next;
				delete current;
			}
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
	Disk disk2{};
	int fileIDs{}; //linesize/2?
	int fileSum{};
	int freeSum{};
	
	for (const auto& line : txt) {
		if (line.length() == 0) { break; }

		bool isEmpty = false;
		for (size_t i = 0; i < line.size(); ++i) {
			int sz = to_int(line[i]);
			if (!isEmpty){
				disk.last = disk.add_block(disk.last, fileIDs, sz);
				disk2.last = disk2.add_block(disk2.last, fileIDs, sz);
				fileIDs++;
				fileSum += sz;
			}
			else {
				disk.last = disk.add_block(disk.last, -1, sz);
				disk2.last = disk2.add_block(disk2.last, -1, sz);
				freeSum += sz;
			}

			isEmpty = !isEmpty;
		}
	}
	
	std::println(std::cout, "fileIDs: {}", fileIDs);
	std::println(std::cout, "fileSum: {}", fileSum);
	std::println(std::cout, "freeSum: {}", freeSum);
	std::println(std::cout, "diskSize: {}", fileSum+freeSum);
	//disk.print();

	std::jthread th1([&disk]() {
		Block* lastBlock = disk.last;
		Block* nextEmpty = disk.root;
		while (lastBlock != disk.root && lastBlock != nullptr) {
			disk.repartition_block(lastBlock, nextEmpty, isNoEmptyBlock);
			//disk.print();
			lastBlock = lastBlock->prev;
		}
		}
	);

	std::jthread th2([&disk2]() {
		Block* lastBlock = disk2.last;
		Block* nextEmpty = disk2.root;
		while (lastBlock != disk2.root && lastBlock != nullptr)
		{
			disk2.repartition_block(lastBlock, nextEmpty, isNoEmptyFittingBlock);
			//disk.print();
			lastBlock = lastBlock->prev;
		}
		}
	);

	th1.join();
	std::println(std::cout, "checksum Disk1 {}", disk.checksum());
	th2.join();
	std::println(std::cout, "checksum Disk2 {}", disk2.checksum());
	
}

