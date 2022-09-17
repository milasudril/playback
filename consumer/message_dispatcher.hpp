#ifndef PLAYBACK_MESSAGEDISPATCHER_HPP
#define PLAYBACK_MESSAGEDISPATCHER_HPP

#include "messages.hpp"

#include "./gl_video_device.hpp"

#include <anon/object.hpp>

#include <span>
#include <string_view>
#include <chrono>

namespace playback
{
	class message_dispatcher
	{
	public:
		explicit message_dispatcher(gl_video_device& video_out):
		m_video_out{video_out}
		{}

		void dispatch(command const& cmd)
		{
			if(cmd.message_type_name == "video_frame_update")
			{
				auto const vfu = deserialize(empty<video_frame_update>{}, cmd.content);
				m_video_out.get().set_pixels(vfu.video_port, cmd.payload);			}
		}

	private:
		std::reference_wrapper<gl_video_device> m_video_out;
	};
}

#endif