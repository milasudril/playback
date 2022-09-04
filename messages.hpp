#ifndef PLAYBACK_MESSAGES_HPP
#define PLAYBACK_MESSAGES_HPP

#include <cstdint>
#include <array>

namespace playback
{
	enum class intensity_transfer_function:int{linear};
	enum class alpha_mode:int{premultiplied, straight};

	struct video_port_config
	{
		uint32_t width;
		uint32_t height;
		std::array<char, 4> channel_layout;
		intensity_transfer_function itf;
		alpha_mode alpha_mode;
	};

	struct video_frame_update
	{
		uint32_t video_port;
	};

	struct
}

#endif