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
