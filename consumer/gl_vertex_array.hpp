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

	class gl_vertex_array
	{
	public:
		explicit gl_vertex_array()
		{
			GLuint handle;
			glCreateVertexArrays(1, &handle);
			m_handle.reset(handle);
		}

		void set_vertex_buffer(GLuint port, gl_buffer const& buffer, GLsizei element_size)
		{
			glVertexArrayVertexBuffer(m_handle.get(), port, buffer.get(), 0, element_size);
			glVertexArrayAttribFormat(m_handle.get(), port, 3, GL_FLOAT, GL_FALSE, 0);
			glEnableVertexArrayAttrib(m_handle.get(), port);
		}

		void bind()
		{
			glBindVertexArray(m_handle.get());
		}


	private:
		gl_vertex_array_handle m_handle;
	};
}

#endif