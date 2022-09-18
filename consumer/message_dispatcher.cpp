//@	{"target":{"name":"message_dispatcher.o"}}

#include "./message_dispatcher.hpp"

#include <optional>

void playback::message_dispatcher::flush_expired_commands(clock::time_point now)
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

void playback::message_dispatcher::dispatch(command const& cmd)
{
	if(cmd.message_type_name == "video_frame_update")
	{
		auto const vfu = deserialize(empty<video_frame_update>{}, cmd.content);
		m_video_out.get().set_pixels(vfu.video_port, cmd.payload);
	}
}