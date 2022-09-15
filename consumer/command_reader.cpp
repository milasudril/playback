//@	{"target":{"name":"command_reader.o"}}

#include "./command_reader.hpp"

void playback::command_reader::read_and_dispatch()
{
	auto& src = m_loader.source();

	if(src.at_eof()) [[unlikely]]
	{ return; }

	switch(m_state)
	{
		case state::message:
			if(auto const res = m_loader.try_read_next<anon::object>(); res.has_value())
			{
				if(auto const i = res->find("payload_size"); i != std::end(*res))
				{
					m_state = state::payload;
					m_msg_type = std::get<std::string>((*res)["message_type"]);
					m_content = std::get<anon::object>((*res)["content"]);
					m_bytes_to_read = std::get<uint64_t>(i->second);
					m_state = state::payload;
				}
				else
				{
					m_dispatcher.get().dispatch(
						std::get<std::string>((*res)["message_type"]),
						std::get<anon::object>((*res)["content"]),
						std::span<std::byte>{});
				}
			}
			break;

		case state::payload:
		{
			auto const res = src.read_into(m_buffer, m_bytes_to_read);
			switch(res.status)
			{
				case read_status::ready:
				case read_status::eof:
					if(res.bytes_to_read == 0)
					{
						m_dispatcher.get().dispatch(m_msg_type, m_content, m_buffer);
						m_state = state::message;
						m_buffer.clear();
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
}