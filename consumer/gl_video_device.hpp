#ifndef PLAYBACK_GLVIDEODEVICE_HPP
#define PLAYBACK_GLVIDEODEVICE_HPP

#include "./gl_viewport.hpp"
#include "./gl_video_port.hpp"

namespace playback
{
	class gl_video_device
	{
	public:
	private:
		gl_viewport m_viewport;
		std::vector<gl_video_port> m_video_ports;
	};
};

#endif
