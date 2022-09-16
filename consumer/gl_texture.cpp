//@	{"target":{"name":"gl_texture.o"}}

#include "./gl_texture.hpp"

GLenum playback::gl_make_sized_format_red(GLenum type)
{
	switch(type)
	{
		case GL_FLOAT:
			return GL_R32F;
		default:
			throw std::runtime_error{"Unimplemented type"};
	}
}

GLenum playback::gl_make_sized_format_rg(GLenum type)
{
	switch(type)
	{
		case GL_FLOAT:
			return GL_RG32F;
		default:
			throw std::runtime_error{"Unimplemented type"};
	}
}

GLenum playback::gl_make_sized_format_rgb(GLenum type)
{
	switch(type)
	{
		case GL_FLOAT:
			return GL_RGB32F;
		default:
			throw std::runtime_error{"Unimplemented type"};
	}
}

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
		case GL_RED:
			return gl_make_sized_format_red(type);
		case GL_RG:
			return gl_make_sized_format_rg(type);
		case GL_RGB:
			return gl_make_sized_format_rgb(type);
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
		case GL_RED:
			return 1*gl_get_sample_size(type);
		case GL_RG:
			return 2*gl_get_sample_size(type);
		case GL_RGB:
			return 3*gl_get_sample_size(type);
		case GL_RGBA:
			return 4*gl_get_sample_size(type);
		default:
			throw std::runtime_error{"Unimplemented format"};
	}
}
