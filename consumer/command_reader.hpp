//@	{"dependencies_extra":[{"ref":"./command_reader.o", "rel":"implementation"}]}

#ifndef PLAYBACK_COMMANDREADER_HPP
#define PLAYBACK_COMMANDREADER_HPP

#include "./fd_reader.hpp"
#include "./message_dispatcher.hpp"

#include <functional>
#include <string>
#include <vector>

namespace playback
{
	class command_reader
	{
	public:
		explicit command_reader(fd_reader& reader,
			message_dispatcher& dispatcher):
			m_loader{reader},
			m_dispatcher{dispatcher},
			m_state{state::message}
		{}

		bool read_and_dispatch();

		void flush_expired_commands(std::chrono::steady_clock::time_point now)
		{ m_dispatcher.get().flush_expired_commands(now); }

	private:
		enum class state : int{message, payload};
		anon::async_loader<fd_reader&> m_loader;
		std::reference_wrapper<message_dispatcher> m_dispatcher;
		state m_state;

		command m_cmd;
		std::byte* m_buffer_ptr;
		uint64_t m_bytes_to_read;
	};
}
#endif