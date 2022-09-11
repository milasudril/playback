#ifndef PLAYBACK_GLVIDEOPORT_HPP
#define PLAYBACK_GLVIDEOPORT_HPP

#include "./gl_mesh.hpp"
#include "./gl_texture.hpp"
#include "messages.hpp"

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

	constexpr auto make_gl_format(video_channel_layout channel_layout)
	{
		switch(channel_layout)
		{
			case video_channel_layout::y:
				return GL_RED;
			case video_channel_layout::ya:
				return GL_RG;
			case video_channel_layout::rgb:
				return GL_RGB;
			case video_channel_layout::rgba:
				return GL_RGBA;
		}
		__builtin_unreachable();
	}

	constexpr auto make_gl_type_id(sample_type type)
	{
		switch(type)
		{
			case sample_type::i8:
				return to_gl_type_id<int8_t>::value;
			case sample_type::u8:
				return to_gl_type_id<uint8_t>::value;
			case sample_type::i16:
				return to_gl_type_id<int16_t>::value;
			case sample_type::u16:
				return to_gl_type_id<uint16_t>::value;
			case sample_type::f32:
				return to_gl_type_id<float>::value;
		}
		__builtin_unreachable();
	}

	constexpr auto make_gl_texture_descriptor(video_port_config const& cfg)
	{
		gl_texture_descriptor ret{};
		ret.width = cfg.width;
		ret.height = cfg.height;
		ret.format = make_gl_format(cfg.channel_layout);
		ret.type = make_gl_type_id(cfg.sample_type);
		ret.num_mipmaps = cfg.num_mipmaps;
		return ret;
	}

	class gl_video_port
	{
	public:
		static constexpr auto default_mesh = image_plane;

		explicit gl_video_port(gl_texture_descriptor const& descriptor):
		m_canvas{default_mesh},
		m_paint{descriptor}
		{}

		explicit gl_video_port(video_port_config const& cfg):
		m_canvas{default_mesh},
		m_paint{make_gl_texture_descriptor(cfg)}
		{}

		gl_video_port():m_canvas{default_mesh}
		{
			auto const test_pattern =
				playback::load_binary<pixel_store::rgba_value<>>("/usr/share/test_pattern/test_pattern.rgba");
			m_paint.upload(pixel_store::image_span{std::data(test_pattern), 1600, 1000}, 10);
		}

		void upload_texture(std::span<std::byte const> data, gl_texture_descriptor const& descriptor)
		{
			m_paint.upload(data, descriptor);
		}

		template<class T>
		void upload_texture(pixel_store::image_span<T const> pixels, GLsizei num_mipmaps)
		{
			m_paint.upload(pixels, num_mipmaps);
		}

		void upload_texture(std::span<std::byte const> data)
		{
			m_paint.upload(data);
		}

		auto const& get_texture_descriptor() const
		{ return m_paint.descriptor(); }

		void bind() const
		{
			m_canvas.bind();
			m_paint.bind(GL_TEXTURE0);
			auto const scale = aspect_ratio(get_texture_descriptor());
			glUniform3f(3, scale, 1.0f, 1.0f);
		}

		size_t get_index_count() const
		{ return m_canvas.get_index_count(); }

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
