//@	{"target":{"name":"fdreader.o"}}

#include "./fd_reader.hpp"

#include <unistd.h>
#include <cstring>

namespace
{
	std::pair<playback::read_status, size_t> do_read(int fd, std::byte* buffer, size_t buffer_size)
	{
		auto res = ::read(fd, buffer, buffer_size);
		switch(res)
		{
			case 0:
				return {playback::read_status::eof, res};

			case -1:
				return {playback::read_status::blocking, res};

			default:
				return {playback::read_status::ready, res};
		}
	}
}

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
	auto res = do_read(m_fd, std::data(m_buffer), std::size(m_buffer));
	if(res.first == read_status::ready)
	{
		m_read_ptr = std::data(m_buffer);
		m_end_ptr = m_read_ptr + res.second;
	}
	return res.first;
}