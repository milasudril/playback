#ifndef PLAYBACK_MESSAGEDISPATCHER_HPP
#define PLAYBACK_MESSAGEDISPATCHER_HPP

#include "messages.hpp"

#include "./gl_video_device.hpp"

#include <anon/object.hpp>
#include <queue>

namespace playback
{
	class message_dispatcher
	{
	public:
		explicit message_dispatcher(gl_video_device& video_out):
		m_video_out{video_out},
		m_last_event{std::chrono::steady_clock::now()}
		{}

		void dispatch(command const& cmd)
		{
			if(cmd.message_type_name == "video_frame_update")
			{
				auto const vfu = deserialize(empty<video_frame_update>{}, cmd.content);
				m_video_out.get().set_pixels(vfu.video_port, cmd.payload);
			}
		}

		void enqueue(command&& cmd)
		{
			m_commands.push(std::move(cmd));
		}

		void flush_expired_commands(std::chrono::steady_clock::time_point now)
		{
			auto last_event = m_last_event;
			while(!m_commands.empty())
			{
				auto const delay = m_commands.front().delay;
				auto const current_event = last_event + delay;
				if(current_event <= now)
				{
					auto cmd = std::move(m_commands.front());
					m_commands.pop();
					last_event = current_event;
					dispatch(cmd);
				}
				else
				{
					m_last_event = last_event;
					return;
				}
			}
		}

	private:
		std::reference_wrapper<gl_video_device> m_video_out;
		std::queue<command> m_commands;
		std::chrono::steady_clock::time_point m_last_event;
	};
}

#endif