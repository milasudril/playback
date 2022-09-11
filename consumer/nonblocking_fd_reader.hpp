#ifndef PLAYBACK_NONBLOCKING_FD_READER_HPP
#define PLAYBACK_NONBLOCKING_FD_READER_HPP

#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace playback
{
	class nonblocking_fd_reader
	{
	public:
		nonblocking_fd_reader(nonblocking_fd_reader const&) = delete;
		nonblocking_fd_reader(nonblocking_fd_reader&&) = delete;
		nonblocking_fd_reader& operator=(nonblocking_fd_reader const&) = delete;
		nonblocking_fd_reader& operator=(nonblocking_fd_reader&&) = delete;

		explicit nonblocking_fd_reader(int fd):
			m_fd{fd},
			m_old_flags{fcntl(fd, F_GETFD)}
		{
			auto res = fcntl(m_fd, F_SETFD, m_old_flags | O_NONBLOCK);
			if(res == -1)
			{ throw std::runtime_error{strerror(errno)}; }
		}

		~nonblocking_fd_reader()
		{
			fcntl(m_fd, F_SETFD, m_old_flags);
		}

		int read(std::span<std::byte> buffer)
		{
			return ::read(m_fd, std::data(buffer), std::size(buffer));
		}

	private:
		int m_fd;
		int m_old_flags;
	};
}

#endif