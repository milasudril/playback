#ifndef PLAYBACK_FDREADER_HPP
#define PLAYBACK_FDREADER_HPP

namespace playback
{
	enum class read_status:uint8_t{success, eof, blocked};

	struct read_byte_result
	{
		std::byte value;
		read_status status;
	}

	class fd_reader
	{
	public:
		read_buffer(int fd):
			m_fd{fd}
			m_read_ptr{std::data(m_buffer)}
			m_end_ptr{std::data(m_buffer)}
		{}

		read_buffer(read_buffer const&) = delete;
		read_buffer(read_buffer&&) = delete;
		read_buffer& operator=(read_buffer const&) = delete;
		read_buffer& operator=(read_buffer&&) = delete;

		auto read_byte()
		{
			if(m_read_ptr == m_ptr)
			{
				auto res = fetch();
				if(res < 0)
				{
					return make_read_status(res);
				}
			}

			auto ret = *m_read_ptr;
			++m_read_ptr;
			return make_read_status(ret);
		}

		auto read_into(std::vector<std::byte>& buffer, size_t bytes_to_read)
		{
			while(bytes_to_read != 0)
			{
				if(m_read_ptr == m_ptr)
				{
					auto res = fetch();
					if(res <= 0)
					{ return make_read_result(bytes_to_read, res); }
				}

				auto const n = std::min(static_cast<size_t>(m_end_ptr - m_read_ptr),
					bytes_to_read);
				std::copy_n(m_read_ptr, n, std::back_inserter(buffer));
				m_end_ptr += n;
				bytes_to_read -= n;
			}

			return make_read_result(bytes_to_read);
		}

	private:
		int m_fd;
		std::array<std::byte, 65536> m_buffer;
		std::byte const* m_read_ptr;
		std::byte const* m_end_ptr;
	}
}

#endif
