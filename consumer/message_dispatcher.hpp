//@	{"dependencies_extra":[{"ref":"./message_dispatcher.o", "rel":"implementation"}]}

#ifndef PLAYBACK_MESSAGEDISPATCHER_HPP
#define PLAYBACK_MESSAGEDISPATCHER_HPP

#include "./gl_video_device.hpp"
#include "./threadsafe_queue.hpp"

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
			m_est_queue_size{0},
			m_should_terminate{false}
		{}

		void dispatch(command const& cmd);

		void enqueue(command&& cmd)
		{
			if(m_should_terminate)
			{ return; }

			std::unique_lock lock{m_size_mtx};
			m_size_cv.wait(lock, [this]() {
				return m_est_queue_size < 1024*1024*1024 || m_should_terminate;
			});

			m_est_queue_size += std::size(cmd.payload) + sizeof(anon::object);
			m_commands.push(std::move(cmd));
		}

		void flush_expired_commands(clock::time_point now);

		void set_should_terminate()
		{
			m_should_terminate = true;
			m_size_cv.notify_one();
		}

	private:
		std::reference_wrapper<gl_video_device> m_video_out;
		clock::time_point m_last_event;
		std::condition_variable m_size_cv;
		std::mutex m_size_mtx;
		size_t m_est_queue_size;
		bool m_should_terminate;
		threadsafe_queue<command> m_commands;
	};
}

#endif