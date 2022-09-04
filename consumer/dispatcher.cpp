//@	{"target":{"name":"dispatcher.o", "pkgconfig_libs":["anon"]}}

#include "./dispatcher.hpp"

#include <anon/deserializer.hpp>

void playback::dispatch(device& dev, anon::object const& message, std::span<std::byte> payload)
{
	auto const& message_type = std::get<std::string>(message["message_type"]);
	if(message_type == "video_frame_update")
	{
		dev.process(deserialize(empty<video_frame_update>{},
			std::get<anon::object>(message["content"])),
			payload);
	}
	else
	{
		fprintf(stderr, "(!) Discarded message of unsupported type %s\n", message_type.c_str());
	}
}

void playback::dispatch(FILE* src, device& dev)
{
	dev.process(deserialize(empty<stream_config>{}, anon::load(src)));

	while(!feof(src))
	{
		auto const& message = anon::load(src);
		if(auto const i = message.find("payload_size"); i != std::end(message))
		{
			auto const payload_size = std::get<size_t>(i->second);
			auto const buffer = std::make_unique<std::byte[]>(payload_size);
			if(fread(buffer.get(), 1, payload_size, src) != payload_size)
			{
				fprintf(stderr, "(!) Stream ended with a truncated messsage\n");
				return;
			}
			dispatch(dev, message, std::span{buffer.get(), payload_size});
		}
		else
		{
			dispatch(dev, message, std::span<std::byte>{});
		}
	}
}