#pragma once
#include <mutex>
#include <deque>
#include <optional>

template<typename TransmissionData>
struct AsyncQueue
{
	std::mutex mut{};
	std::deque<TransmissionData> data{};

	void push(TransmissionData&& input)
	{
		std::lock_guard guard(mut);
		data.emplace_back(input);
	}

	void push(TransmissionData const& input)
	{
		std::lock_guard guard(mut);
		data.emplace_back(input);
	}

	std::optional<TransmissionData> pop()
	{
		std::lock_guard guard(mut);
		if (data.size() > 0)
		{
			TransmissionData ret = data.front();
			data.pop_front();
			return ret;
		}
		return {};
	}

	[[nodiscard]] bool empty() {
		std::lock_guard guard(mut);
		return data.empty();
	}
};