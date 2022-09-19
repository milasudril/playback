//@	{"target":{"name":"command_reader.o"}}

#include "./command_reader.hpp"

namespace
{
	auto to_steady_clock_duration(double value)
	{
		std::chrono::duration<double> t{value};
		using duration_type = std::chrono::steady_clock::duration;
		return std::chrono::duration_cast<duration_type>(t);
	}
}

bool playback::command_reader::read_and_dispatch()
{
	auto& src = m_loader.source();

	if(src.at_eof()) [[unlikely]]
	{ return true; }

	switch(m_state)
	{
		case state::message:
			if(auto const res = m_loader.try_read_next<anon::object>(); res.has_value())
			{
				m_cmd.message_type_name = std::get<std::string>((*res)["message_type_name"]);
				m_cmd.delay = to_steady_clock_duration(std::get<double>((*res)["delay"]));
				m_cmd.content = std::get<anon::object>((*res)["content"]);

				if(auto const i = res->find("payload_size"); i != std::end(*res))
				{
					m_state = state::payload;

					m_bytes_to_read = std::get<uint64_t>(i->second);
					m_cmd.payload = uninitialized_buffer<std::byte>{m_bytes_to_read};
					m_buffer_ptr = std::data(m_cmd.payload);
					m_state = state::payload;
				}
				else
				{
					m_dispatcher.get().enqueue(std::move(m_cmd));
				}
			}
			break;

		case state::payload:
		{
			auto const res = src.read_into(std::ref(m_buffer_ptr), m_bytes_to_read);
			switch(res.status)
			{
				case read_status::ready:
				case read_status::eof:
					if(res.bytes_to_read == 0)
					{
						m_dispatcher.get().enqueue(std::move(m_cmd));
						m_state = state::message;
					}
					break;

				case read_status::blocking:
					m_bytes_to_read = res.bytes_to_read;
					break;

				default:
					__builtin_unreachable();
			}
			break;
		}

		default:
			__builtin_unreachable();
	}
	return false;
}