//@	{"dependencies_extra":[{"ref":"./gl_texture.o", "rel":"implementation"}]}

#ifndef PLAYBACK_GLTEXTURE_HPP
#define PLAYBACK_GLTEXTURE_HPP

#include "./gl_resource.hpp"
#include "./gl_types.hpp"
#include "pixel_store/image_span.hpp"
#include "pixel_store/rgba_value.hpp"

namespace playback
{
	struct gl_texture_deleter
	{
		void operator()(GLuint handle) const
		{
			glDeleteTextures(1, &handle);
		}
	};

	using gl_texture_handle = gl_resource<gl_texture_deleter>;

	struct gl_texture_descriptor
	{
		GLsizei width;
		GLsizei height;
		GLenum format;
		GLenum type;
		GLsizei num_mipmaps;

		auto operator<=>(gl_texture_descriptor const& other) const = default;
	};

	template<class T>
	struct to_gl_color_channel_layout;

	template<>
	struct to_gl_color_channel_layout<pixel_store::rgba_value<>>
	{
		static constexpr auto value = GL_RGBA;
	};

	GLenum gl_make_sized_format_rgba(GLenum type);

	GLenum gl_make_sized_format(GLenum format, GLenum type);

	size_t gl_get_pixel_size(GLenum format, GLenum type);

	inline auto get_image_size(gl_texture_descriptor const& descriptor)
	{
		return descriptor.width*descriptor.height*gl_get_pixel_size(descriptor.format, descriptor.type);
	}

	class gl_texture
	{
	public:
		explicit gl_texture():m_descriptor{0, 0, 0, 0, 0}{}

		void upload(std::span<std::byte const> data, gl_texture_descriptor const& descriptor)
		{
			if(descriptor != m_descriptor) [[unlikely]]
			{
				set_format(descriptor);
			}

			upload_impl(data);
		}

		template<class T>
		void upload(pixel_store::image_span<T const> pixels, GLsizei num_mipmaps)
		{
			gl_texture_descriptor const descriptor{
				static_cast<GLsizei>(pixels.width()),
				static_cast<GLsizei>(pixels.height()),
				to_gl_color_channel_layout<T>::value,
				to_gl_type_id_v<T>,
				num_mipmaps
			};

			std::span const pixel_array{std::data(pixels), pixels.width()*pixels.height()};

			upload(std::as_bytes(pixel_array), descriptor);
		}

		void set_format(gl_texture_descriptor const& descriptor)
		{
			GLuint handle;
			glCreateTextures(GL_TEXTURE_2D, 1, &handle);
			glTextureStorage2D(handle,
				descriptor.num_mipmaps,
				gl_make_sized_format(descriptor.format, descriptor.type),
				descriptor.width,
				descriptor.height);
			m_handle.reset(handle);
			m_descriptor = descriptor;
		}

		void upload(std::span<std::byte const> data)
		{
			auto const image_size = get_image_size(m_descriptor);
			if(image_size != std::size(data)) [[unlikely]]
			{
				fprintf(stderr, "(!) Ignoring texture data of wrong size\n");
				return;
			}
			upload_impl(data);
		}

		void set_parameter(GLenum name, GLint value)
		{glTextureParameteri(m_handle.get(), name, value);}

		void set_parameter(GLenum name, float value)
		{glTextureParameterf(m_handle.get(), name, value);}

		void bind(GLenum texture_unit)
		{
			glActiveTexture(texture_unit);
			glBindTexture(GL_TEXTURE_2D, m_handle.get());
		}

	private:
		void upload_impl(std::span<std::byte const> data)
		{
			glTextureSubImage2D(m_handle.get(),
				0,  // mipmap level
				0,  // x-offset
				0,  // y-offset
				m_descriptor.width,
				m_descriptor.height,
				m_descriptor.format,
				m_descriptor.type,
				std::data(data));

			if(m_descriptor.num_mipmaps != 0) [[unlikely]]
			{ glGenerateTextureMipmap(m_handle.get()); }
		}

		gl_texture_handle m_handle;
		gl_texture_descriptor m_descriptor;
	};
}

#endif