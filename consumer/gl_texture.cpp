//@	{"target":{"name":"gl_texture.o"}}

#include "./gl_texture.hpp"

GLenum playback::gl_make_sized_format_rgba(GLenum type)
{
	switch(type)
	{
		case GL_FLOAT:
			return GL_RGBA32F;
		default:
			throw std::runtime_error{"Unimplemented type"};
	}
}

GLenum playback::gl_make_sized_format(GLenum format, GLenum type)
{
	switch(format)
	{
		case GL_RGBA:
			return gl_make_sized_format_rgba(type);
		default:
			throw std::runtime_error{"Unimplemented format"};
	}
}

size_t playback::gl_get_sample_size(GLenum type)
{
	switch(type)
	{
		case GL_FLOAT:
			return 4;
		default:
			throw std::runtime_error{"Unimplemented type"};
	}
}

size_t playback::gl_get_pixel_size(GLenum format, GLenum type)
{
	switch(format)
	{
		case GL_RGBA:
			return 4*gl_get_sample_size(type);
		default:
			throw std::runtime_error{"Unimplemented format"};
	}
}
