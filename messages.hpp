#ifndef PLAYBACK_MESSAGES_HPP
#define PLAYBACK_MESSAGES_HPP

#include <anon/object.hpp>

#include <cstdint>
#include <array>
#include <vector>

namespace playback
{
	enum class intensity_transfer_function:int{linear};
	enum class alpha_mode:int{premultiplied, straight};

	struct video_port_config
	{
		explicit video_port_config(anon::object const&);

		uint32_t width;
		uint32_t height;
		std::array<char, 4> channel_layout;
		intensity_transfer_function itf;
		alpha_mode am;
	};

	struct stream_config
	{
		explicit stream_config(anon::object const&);
		std::vector<video_port_config> video_ports;
	};

	struct video_frame_update
	{
		explicit video_frame_update(anon::object const&);
		uint32_t video_port;
	};
}

#endif