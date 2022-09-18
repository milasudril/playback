#ifndef PLAYBACK_MESSAGEWRITER_HPP
#define PLAYBACK_MESSAGEWRITER_HPP

#include "./messages.hpp"

#include <anon/serializer.hpp>

#include <span>
#include <cstdio>

namespace playback
{
	template<class T>
	void write(T const& obj, FILE* stream)
	{
		store(serialize(obj), stream);
	}

	void write(std::span<std::byte const> data, FILE* stream)
	{
		fwrite(std::data(data), 1, std::size(data), stream);
	}

	template<class T>
	void write_command(T const& content,
		std::span<std::byte const> payload,
		std::chrono::duration<double> delay,
		FILE* stream)
	{
		store(anon::object{}
			.insert_or_assign("message_type_name", T::message_type_name)
			.insert_or_assign("payload_size", static_cast<uint64_t>(std::size(payload)))
			.insert_or_assign("delay", delay.count())
			.insert_or_assign("content", serialize(content)), stream);
		write(payload, stream);
	}
}

#endif