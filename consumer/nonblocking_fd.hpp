#ifndef PLAYBACK_NONBLOCKING_FD_HPP
#define PLAYBACK_NONBLOCKING_FD_HPP

#include <unistd.h>
#include <fcntl.h>
#include <cstring>

namespace playback
{
	class nonblocking_fd
	{
	public:
		nonblocking_fd(nonblocking_fd const&) = delete;
		nonblocking_fd(nonblocking_fd&&) = delete;
		nonblocking_fd& operator=(nonblocking_fd const&) = delete;
		nonblocking_fd& operator=(nonblocking_fd&&) = delete;

		explicit nonblocking_fd(int fd):
			m_fd{fd},
			m_old_flags{fcntl(fd, F_GETFL)}
		{
			auto res = fcntl(m_fd, F_SETFL, m_old_flags | O_NONBLOCK);
			if(res == -1)
			{ throw std::runtime_error{strerror(errno)}; }
		}

		~nonblocking_fd()
		{
			fcntl(m_fd, F_SETFL, m_old_flags);
		}

	private:
		int m_fd;
		int m_old_flags;
	};
}

#endif