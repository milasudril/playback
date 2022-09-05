#ifndef PLAYBACK_GLVIDEOPORT_HPP
#define PLAYBACK_GLVIDEOPORT_HPP

#include "messages.hpp"

#include "./gl_buffer.hpp"
#include "./gl_vertex_array.hpp"

namespace playback
{
#if 0
	auto to_gl_format(video_channel_layout layout)
	{
		switch(layout)
		{
			case channel_layout::y:
				return GL_RED;
			case channel_layout::ya:
				return GL_RG;
			case channel_layout::rgb:
				return GL_RGB;
			case channel_layout::rgba:
				return GL_RGBA;
		}
		__builtin_unreachable();
	}
#endif

	class gl_video_port
	{
	public:
#if 0
		gl_video_port& configure(video_port_config const& cfg)
		{
			if(m_config != cfg)
			{
				m_texture = create_texture(cfg);
				m_config = cfg;
			}

			return *this;
		}

		gl_video_port& upload_pixels(std::span<std::byte const> pixels) const
		{
			auto const pixel_size = get_pixel_size(m_config);
			auto const pixel_count = get_pixel_count(m_config);
			if(pixel_size*pixel_count != std::size(pixels))
			{
				fprintf(stderr, "(!) Ignoring frame of incorrect size\n");
				return *this;
			}

			glTextureSubimage2d(m_texture.get(), 0,
				0, 0, m_config.width, m_config.height,
				to_gl_format(m_config.channel_layout),
				to_gl_type(m_config.value_type)
				std::data(pixels));
			return *this;
		}
#endif

	private:
		gl_buffer m_vertices;
		gl_buffer m_uvs;
		gl_buffer m_faces;
		gl_vertex_array m_verex_array;
	//	gl_texture m_texture;
		video_port_config m_config;
	};
}

#endif