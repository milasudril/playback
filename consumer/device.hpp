# ifndef PLAYBACK_DEVICE_HPP
#define PLAYBACK_DEVICE_HPP

#include "messages.hpp"

#include <span>

namespace playback
{
	class device
	{
	public:
		virtual void process(video_frame_update const&, std::span<std::byte const>) = 0;
		virtual void process(stream_config const&) = 0;
	};
}

#endif
