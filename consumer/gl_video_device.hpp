#ifndef PLAYBACK_GLVIDEODEVICE_HPP
#define PLAYBACK_GLVIDEODEVICE_HPP

#include "./gl_viewport.hpp"
#include "./gl_video_port.hpp"
#include "messages.hpp"

#include <algorithm>

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

		void configure_port(size_t index, video_port_config const& cfg)
		{
			 if(index >= std::size(m_video_ports)) [[unlikely]]
			 { m_video_ports.resize(index + 1); }

			 m_video_ports[index] = gl_video_port{cfg};
		}

		void set_pixels(size_t index, std::span<std::byte const> data)
		{
			if(index >= std::size(m_video_ports)) [[unlikely]]
			{
				fprintf(stderr, "(!) trying to upload pixles to non-existing video port\n");
				return;
			}
			m_video_ports[index].upload_texture(data);
		}

		void render_content() const
		{
			std::ranges::for_each(m_video_ports, [](auto const& item) {
				item.bind();
				glDrawElements(GL_TRIANGLES, item.get_index_count(), playback::gl_bindings::vertex_index_type(), nullptr);
			});
		}

	private:
		std::vector<gl_video_port> m_video_ports;
	};
};

#endif
