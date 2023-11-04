#pragma once
#include <string_view>
#include <atomic>
#include <mutex>
#include <optional>

template<char SplitChar>
struct SplitIterator
{
	using iterator_category = std::forward_iterator_tag;
	using value_type = std::string_view;
	using difference_type = std::ptrdiff_t;
	using reference_type = value_type const&;
	using pointer_type = const value_type*;

	std::optional<std::string_view> theText{};//container
	value_type theLine{};//value (*it)
	size_t start_pos{};
	size_t end_pos{};
	//ctor "end"
	SplitIterator()
	{}
	//ctor "begin"
	SplitIterator(reference_type txt)
		: theText(txt)
	{
		++*this;
	}

	//retrieve
	reference_type operator*() const { return theLine; }
	pointer_type operator->() const { return &theLine; }

	//assign
	SplitIterator& operator=(reference_type vw)
	{
		theText = vw;
		start_pos = 0LL;
		end_pos = 0LL;
		return *this;
	}
	SplitIterator& operator=(SplitIterator const& ref)
	{
		theText = ref.theText;
		theLine = ref.theLine;
		start_pos = ref.start_pos;
		end_pos = ref.end_pos;
		return *this;
	}

	//input_iterator_tag,
	//++it
	SplitIterator& operator++() {
		theLine = {};
		if (theText) {
			end_pos = theText->find(SplitChar, start_pos);
			if(end_pos!=std::string_view::npos){
				theLine = theText->substr(start_pos, end_pos - start_pos);
				start_pos = end_pos + 1;
			}
			else {
				if ( start_pos<theText->length() )
				{
					theLine = theText->substr(start_pos);
					start_pos = theText->length();
				}
				else
				{
					theText = {};
				}
			}
		}
		return *this;
	}
	//it++
	SplitIterator& operator++(int) {
		SplitIterator elem(*this);
		++*this;
		return elem;
	}
	//compare
	friend bool operator==(SplitIterator const& lhs, SplitIterator const& rhs)
	{
		return lhs.theText == rhs.theText;
	}

};



