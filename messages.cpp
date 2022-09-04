//@	{"target":{"name":"messages.o"}}

#include "messages.hpp"

playback::intensity_transfer_function playback::deserialize(empty<intensity_transfer_function>,
	std::string_view str)
{
	if(str == "linear")
	{
		return intensity_transfer_function::linear;
	}

	throw std::runtime_error{"Unsupported intensity transfer function"};
}

char const* playback::to_string(intensity_transfer_function val)
{
	switch(val)
	{
		case intensity_transfer_function::linear:
			return "linear";
	}
	__builtin_unreachable();
}

	enum class alpha_mode:int{premultiplied, straight};

playback::alpha_mode playback::deserialize(empty<alpha_mode>, std::string_view str)
{
	if(str == "premultiplied")
	{ return alpha_mode::premultiplied; }

	if(str == "straight")
	{ return alpha_mode::straight; }

	throw std::runtime_error{"Unsupported alpha mode"};
}

char const* playback::to_string(alpha_mode val)
{
	switch(val)
	{
		case alpha_mode::premultiplied:
			return "premultiplied";
		case alpha_mode::straight:
			return "straight";
	}
	__builtin_unreachable();
}

playback::channel_layout playback::deserialize(empty<channel_layout>, std::string_view str)
{
	if(str == "y")
	{ return channel_layout::y; }

	if(str == "ya")
	{ return channel_layout::ya; }

	if(str == "rgb")
	{ return channel_layout::rgb; }

	if(str == "rgba")
	{ return channel_layout::rgba; }

	throw std::runtime_error{"Unsupported channel layout"};
}

char const* playback::to_string(channel_layout val)
{
	switch(val)
	{
		case channel_layout::y:
			return "y";
		case channel_layout::ya:
			return "ya";
		case channel_layout::rgb:
			return "rgb";
		case channel_layout::rgba:
			return "rgba";
	}
	__builtin_unreachable();
}

playback::video_port_config playback::deserialize(empty<video_port_config>, anon::object const& obj)
{
	video_port_config ret{};
	ret.width = std::get<uint32_t>(obj["width"]);
	ret.height = std::get<uint32_t>(obj["height"]);
	ret.channel_layout = deserialize(empty<channel_layout>{},
		std::get<std::string>(obj["channel_layout"]));
	ret.intensity_transfer_function = deserialize(empty<intensity_transfer_function>{},
		std::get<std::string>(obj["intensity_transfer_function"]));
	ret.alpha_mode = deserialize(empty<alpha_mode>{}, std::get<std::string>(obj["alpha_mode"]));

	return ret;
}

anon::object playback::serialize(video_port_config const& cfg)
{
	return anon::object{}
		.insert_or_assign("width", cfg.width)
		.insert_or_assign("height", cfg.height)
		.insert_or_assign("channel_layout", to_string(cfg.channel_layout))
		.insert_or_assign("intensity_transfer_function", to_string(cfg.intensity_transfer_function))
		.insert_or_assign("alpha_mode", to_string(cfg.alpha_mode));
}

playback::stream_config playback::deserialize(empty<stream_config>, anon::object const& obj)
{
	stream_config ret{};
	std::ranges::transform(std::get<std::vector<anon::object>>(obj["video_ports"]),
		std::back_inserter(ret.video_ports),
		[](auto const& item) {
			return deserialize(empty<video_port_config>{}, item);
	});
	return ret;
}

anon::object playback::serialize(stream_config const& cfg)
{
	std::vector<anon::object> video_ports;
	std::ranges::transform(cfg.video_ports, std::back_inserter(video_ports), [](auto const& item){
		return serialize(item);
	});

	return anon::object{}.insert_or_assign("video_ports", std::move(video_ports));
}

struct video_frame_update
{
	uint32_t video_port;
};

playback::video_frame_update playback::deserialize(empty<video_frame_update>, anon::object const& obj)
{
	video_frame_update ret{};
	ret.video_port = std::get<uint32_t>(obj["video_port"]);
	return ret;
}

anon::object playback::serialize(playback::video_frame_update const& msg)
{
	return anon::object{}.insert_or_assign("video_port", msg.video_port);
}