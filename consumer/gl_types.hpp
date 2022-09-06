#ifndef PLAYBACK_GLTYPES_HPP
#define PLAYBACK_GLTYPES_HPP

#include <GL/glew.h>
#include <GL/gl.h>

namespace playback
{
	template<class T>
	struct to_gl_type_id
	{
		static constexpr auto value = to_gl_type_id<typename T::value_type>::value;
	};

	template<>
	struct to_gl_type_id<float>
	{
		static constexpr auto value = GL_FLOAT;
	};

	template<>
	struct to_gl_type_id<unsigned int>
	{
		static constexpr auto value = GL_UNSIGNED_INT;
	};

	template<>
	struct to_gl_type_id<unsigned short>
	{
		static constexpr auto value = GL_UNSIGNED_SHORT;
	};

	template<class T>
	constexpr auto to_gl_type_id_v = to_gl_type_id<T>::value;
}

#endif