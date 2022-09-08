#ifndef PLAYBACK_GLVIDEOPORT_HPP
#define PLAYBACK_GLVIDEOPORT_HPP

#include "./gl_mesh.hpp"
#include "./gl_texture.hpp"

namespace playback
{	
	template<class T>
	using array_of_4 = std::array<T, 4>;
	
	inline constexpr mesh<array_of_4, std::array<unsigned int, 6>> image_plane
	{
		array_of_4<point>{},
		array_of_4<uv_coords>{},
		std::array<unsigned int, 6>{}
	};
	
	class gl_video_port
	{
	public:
		static constexpr auto default_mesh = image_plane;
		
	private:
		gl_mesh<unsigned int> m_canvas;
		gl_texture m_paint;
	};
}

#endif
