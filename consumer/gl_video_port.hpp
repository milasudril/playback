#ifndef PLAYBACK_GLVIDEOPORT_HPP
#define PLAYBACK_GLVIDEOPORT_HPP

#include "./gl_mesh.hpp"
#include "./gl_texture.hpp"

#include "io_utils.hpp"

namespace playback
{	
	template<class T>
	using array_of_4 = std::array<T, 4>;
	
	inline constexpr mesh<array_of_4, std::array<unsigned int, 6>> image_plane
	{
		array_of_4<point>
		{
			point{ 1.0f,  1.0f, 0.0f},
			point{ 1.0f, -1.0f, 0.0f},
			point{-1.0f, -1.0f, 0.0f},
			point{-1.0f,  1.0f, 0.0f}
		},

		array_of_4<uv_coords>
		{
			uv_coords{ 1.0f,  0.0f},
			uv_coords{ 1.0f,  1.0f},
			uv_coords{ 0.0f,  1.0f},
			uv_coords{ 0.0f,  0.0f}						
		},

		std::array<unsigned int, 6>{0, 3, 1, 3, 2, 1}
	};
	
	class gl_video_port
	{
	public:
		static constexpr auto default_mesh = image_plane;
		
		gl_video_port():m_canvas{default_mesh}
		{
			auto const test_pattern =
				playback::load_binary<pixel_store::rgba_value<>>("/usr/share/test_pattern/test_pattern.rgba");
			m_paint.upload(pixel_store::image_span{std::data(test_pattern), 1600, 1000}, 10);
		}
		
		void bind()
		{
			m_canvas.bind();
			m_paint.bind(GL_TEXTURE0);
		}
		
		auto const& get_texture_descriptor() const
		{ return m_paint.descriptor(); }
		
	private:
		gl_mesh<unsigned int> m_canvas;
		gl_texture m_paint;
	};
	
	inline auto aspect_ratio(gl_video_port const& video_port)
	{
		return aspect_ratio(video_port.get_texture_descriptor());
	}
}

#endif
