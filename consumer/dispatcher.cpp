//@	{"target":{"name":"dispatcher.o"}}

#include "./dispatcher.hpp"

#include <anon/deserializer.hpp>

void playback::dispatch(FILE* src, device& dev, anon::object const& message)
{
	auto const& message_type = std::get<std::string>(message["message_type"]);
	if(message_type == "video_frame_update")
	{
		dev.process(video_frame_update{std::get<anon::object>(message["content"])}, src);
	}
}

void playback::dispatch(FILE* src, device& dev)
{
	dev.process(stream_config{anon::load(src)});

	while(!feof(src))
	{
		dispatch(src, dev, anon::load(src));
	}
}