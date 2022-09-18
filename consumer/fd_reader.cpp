//@	{"target":{"name":"fdreader.o"}}

#include "./fd_reader.hpp"

#include <unistd.h>
#include <cstring>

playback::read_data_result playback::fd_reader::read_into(std::reference_wrapper<std::byte*> buffer,
	size_t bytes_to_read)
{
	std::byte* write_ptr = buffer.get();
	while(bytes_to_read != 0)
	{
		if(m_read_ptr == m_end_ptr)
		{
			if(auto const res = fetch(); res != read_status::ready) [[unlikely]]
			{
				buffer.get() = write_ptr;
				return read_data_result{bytes_to_read, res};
			}
		}

		auto const n = std::min(static_cast<size_t>(m_end_ptr - m_read_ptr),
			bytes_to_read);
		memcpy(write_ptr, m_read_ptr, n);
		m_read_ptr += n;
		write_ptr += n;
		bytes_to_read -= n;
	}

	buffer.get() = write_ptr;
	return read_data_result{0, read_status::ready};
}

playback::read_status playback::fd_reader::fetch()
{
	auto res = ::read(m_fd, std::data(m_buffer), std::size(m_buffer));
	switch(res)
	{
		case 0:
			return read_status::eof;

		case -1:
			return read_status::blocking;

		default:
			m_read_ptr = std::data(m_buffer);
			m_end_ptr = m_read_ptr + res;
			return read_status::ready;
	}
}