#ifndef PLAYBACK_GLVIDEODEVICE_HPP
#define PLAYBACK_GLVIDEODEVICE_HPP

#include "./gl_viewport.hpp"
#include "./gl_video_port.hpp"

namespace playback
{
	class gl_video_device:private gl_viewport
	{
	public:
		explicit gl_video_device(glfw_context& ctxt, uint32_t width, uint32_t height):
		gl_viewport{ctxt, width, height, "gl_video_device"}
		{
		}
		
		using gl_viewport::set_event_handler;
		using gl_viewport::activate_gl_context;
		using gl_viewport::swap_buffer;

	private:
		std::vector<gl_video_port> m_video_ports;
	};
};

#endif
