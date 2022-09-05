#ifndef PLAYBACK_GLVERTEXARRAY_HPP
#define PLAYBACK_GLVERTEXARRAY_HPP

#include "./gl_resource.hpp"
#include "./gl_buffer.hpp"

#include <memory>

namespace playback
{
	struct gl_vertex_array_deleter
	{
		void operator()(GLuint handle) const
		{
			glDeleteVertexArrays(1, &handle);
		}
	};

	using gl_vertex_array_handle = gl_resource<gl_vertex_array_deleter>;

	template<class T>
	struct to_gl_type_id;

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

	class gl_bindings
	{
	public:
		static auto vertex_index_type() { return s_bound_index_type; }

	protected:
		thread_local static inline GLenum s_bound_index_type;
	};

	class gl_vertex_array:protected gl_bindings
	{
	public:
		explicit gl_vertex_array()
		{
			GLuint handle;
			glCreateVertexArrays(1, &handle);
			m_handle.reset(handle);
		}

		template<class T>
		void set_buffer(GLuint port, gl_vertex_buffer<T> const& buffer)
		{
			using scalar_type = typename T::scalar_type;

			glVertexArrayVertexBuffer(m_handle.get(), port, buffer.get(), 0, sizeof(T));
			glVertexArrayAttribFormat(m_handle.get(), port, 3, to_gl_type_id_v<scalar_type>, GL_FALSE, 0);
			glEnableVertexArrayAttrib(m_handle.get(), port);
		}

		template<class T>
		void set_buffer(gl_index_buffer<T> const& buffer)
		{
			glVertexArrayElementBuffer(m_handle.get(), buffer.get());
			m_index_type = to_gl_type_id_v<T>;
		}

		void bind()
		{
			glBindVertexArray(m_handle.get());
			s_bound_index_type = m_index_type;
		}


	private:
		gl_vertex_array_handle m_handle;
		GLenum m_index_type;
	};
}

#endif