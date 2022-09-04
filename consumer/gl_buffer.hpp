#ifndef PLAYBACK_GLBUFFER_HPP
#define PLAYBACK_GLBUFFER_HPP

#include "./gl_resource.hpp"

#include <memory>

namespace playback
{
	struct gl_buffer_deleter
	{
		void operator()(GLuint handle) const
		{
			glDeleteBuffers(1, &handle);
		}
	};

	using gl_buffer_handle = gl_resource<gl_buffer_deleter>;

	class gl_buffer
	{
	public:
		explicit gl_buffer():m_capacity{0}
		{
			GLuint buffer;
			glCreateBuffers(1, & buffer);
			m_buffer.reset(buffer);
		}

		void upload(std::span<std::byte const> data)
		{
			if(std::size(data) != m_capacity)
			{
				glNamedBufferStorage(m_buffer.get(), std::size(data), nullptr, GL_DYNAMIC_STORAGE_BIT);
			}
			glNamedBufferSubData(m_buffer.get(), 0, std::size(data), std::data(data));
		}

		void bind_to_array_buffer()
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_buffer.get());
		}

		void bind_to_element_buffer()
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_buffer.get());
		}

		auto get() const { return m_buffer.get(); }

	private:
		gl_buffer_handle m_buffer;
		size_t m_capacity;
	};
}

#endif