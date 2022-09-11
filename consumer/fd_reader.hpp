//@	{"dependencies_extra":[{"ref":"./fdreader.o", "rel":"implementation"}]}

#ifndef PLAYBACK_FDREADER_HPP
#define PLAYBACK_FDREADER_HPP

#include <sys/types.h>

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>

namespace playback
{
	enum class read_status:uint8_t{success, eof, blocked};

	struct read_byte_result
	{
		std::byte value;
		read_status status;
	};

	inline auto make_read_result(ssize_t val)
	{
		return read_byte_result{
			.value = std::byte{0x0},
			.status = val == 0 ? read_status::eof : read_status::blocked
		};
	}

	inline auto make_read_result(std::byte val)
	{
		return read_byte_result{.value = val, .status = read_status::success};
	}



	struct read_data_result
	{
		size_t bytes_to_read;
		read_status status;
	};

	inline auto make_read_result(size_t bytes_to_read, ssize_t val)
	{
		return read_data_result{
			.bytes_to_read = bytes_to_read,
			.status = val == 0 ? read_status::eof : read_status::blocked
		};
	}


	class fd_reader
	{
	public:
		explicit fd_reader(int fd):
			m_fd{fd},
			m_read_ptr{std::data(m_buffer)},
			m_end_ptr{std::data(m_buffer)}
		{}

		fd_reader(fd_reader const&) = delete;
		fd_reader(fd_reader&&) = delete;
		fd_reader& operator=(fd_reader const&) = delete;
		fd_reader& operator=(fd_reader&&) = delete;

		read_byte_result read_byte()
		{
			if(m_read_ptr == m_end_ptr) [[unlikely]]
			{
				if(auto const res = fetch(); res != read_status::success) [[unlikely]]
				{
					return read_byte_result{std::byte{0x00}, res};
				}
			}

			auto ret = *m_read_ptr;
			++m_read_ptr;
			return read_byte_result{ret, read_status::success};
		}

		read_data_result read_into(std::vector<std::byte>& buffer, size_t bytes_to_read);

	private:
		read_status fetch();

		int m_fd;
		std::array<std::byte, 65536> m_buffer;
		std::byte const* m_read_ptr;
		std::byte const* m_end_ptr;
	};
}

#endif
