#ifndef PLAYBACK_MESSAGEDISPATCHER_HPP
#define PLAYBACK_MESSAGEDISPATCHER_HPP

#include "./gl_video_device.hpp"
#include "./threadsafe_queue.hpp"

#include "messages.hpp"

#include <anon/object.hpp>
#include <queue>
#include <condition_variable>

namespace playback
{
	class message_dispatcher
	{
	public:
		using clock = std::chrono::steady_clock;

		explicit message_dispatcher(gl_video_device& video_out):
		m_video_out{video_out},
		m_last_event{clock::now()},
		m_est_queue_size{0}
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
			std::unique_lock lock{m_size_mtx};
			m_size_cv.wait(lock, [this]() {
				return m_est_queue_size < 1024*1024*1024;
			});

			m_est_queue_size += std::size(cmd.payload) + sizeof(anon::object);
			m_commands.push(std::move(cmd));
		}

		void flush_expired_commands(clock::time_point now)
		{
			while(true)
			{
				auto res = m_commands.with_lock([last_event = m_last_event, now](auto& queue) {
					if(queue.empty())
					{ return std::pair{std::optional<command>{}, last_event}; }

					auto const delay = queue.front().delay;
					auto const current_event = last_event + delay;
					if(current_event <= now)
					{
						auto ret = std::pair{std::optional{std::move(queue.front())}, current_event};
						queue.pop();
						return ret;
					}
					else
					{ return std::pair{std::optional<command>{}, last_event}; }
				}) ;

				if(res.first.has_value())
				{
					auto const& cmd = *res.first;
					std::lock_guard lock{m_size_mtx};
					m_est_queue_size -= std::size(cmd.payload) + sizeof(anon::object);
					m_size_cv.notify_one();
					m_last_event = res.second;
					dispatch(cmd);
				}
				else
				{
					return;
				}
			}
		}

	private:
		std::reference_wrapper<gl_video_device> m_video_out;
		clock::time_point m_last_event;
		std::condition_variable m_size_cv;
		std::mutex m_size_mtx;
		size_t m_est_queue_size;
		threadsafe_queue<command> m_commands;
	};
}

#endif