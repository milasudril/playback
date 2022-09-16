#ifndef PLAYBACK_MESSAGEWRITER_HPP
#define PLAYBACK_MESSAGEWRITER_HPP

#include "messages.hpp"

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
	void write_message(T const& message, std::span<std::byte const> payload, FILE* stream)
	{
		store(anon::object{}
			.insert_or_assign("message_type", T::message_type_id)
			.insert_or_assign("payload_size", static_cast<uint64_t>(std::size(payload)))
			.insert_or_assign("content", serialize(message)), stream);
		write(payload, stream);
	}
}

#endif