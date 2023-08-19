#pragma once
#include <string_view>
#include <atomic>
#include <mutex>
#include <optional>

struct LineIterator
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
	LineIterator()
	{}
	//ctor "begin"
	LineIterator(reference_type txt)
		: theText(txt)
	{
		++*this;
	}

	//retrieve
	reference_type operator*() const { return theLine; }
	pointer_type operator->() const { return &theLine; }

	//assign
	LineIterator& operator=(reference_type vw)
	{
		theText = vw;
		start_pos = 0LL;
		end_pos = 0LL;
		return *this;
	}
	LineIterator& operator=(LineIterator const& ref)
	{
		theText = ref.theText;
		theLine = ref.theLine;
		start_pos = ref.start_pos;
		end_pos = ref.end_pos;
		return *this;
	}

	//input_iterator_tag,
	//++it
	LineIterator& operator++() {
		theLine = {};
		if (theText) {
			end_pos = theText->find('\n', start_pos);
			if(end_pos!=std::string_view::npos){
				theLine = theText->substr(start_pos, end_pos - start_pos);
				if (theLine.rbegin()!=theLine.rend() && *theLine.rbegin() == '\r')
				{
					theLine.remove_suffix(1);
				}
				start_pos = end_pos + 1;
			}
			else {
				if ( start_pos<theText->length() )
				{
					theLine = theText->substr(start_pos);
					if (theLine.rbegin() != theLine.rend() && *theLine.rbegin() == '\r')
					{
						theLine.remove_suffix(1);
					}
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
	LineIterator& operator++(int) {
		LineIterator elem(*this);
		++*this;
		return elem;
	}
	//compare
	friend bool operator==(LineIterator const& lhs, LineIterator const& rhs)
	{
		return lhs.theText == rhs.theText;
	}
};



