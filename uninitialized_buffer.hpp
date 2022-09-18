#ifndef PLAYBACK_UNINITIALIZEDBUFFER_HPP
#define PLAYBACK_UNINITIALIZEDBUFFER_HPP

#include <cstddef>
#include <memory>

namespace playback
{
	template<class T>
	class uninitialized_buffer
	{
	public:
		uninitialized_buffer():m_size{0}{}

		explicit uninitialized_buffer(size_t size):
			m_buffer{std::make_unique_for_overwrite<T[]>(size)},
			m_size{size}
		{}

		size_t size() const { return m_size; }

		T const* begin() const { return m_buffer.get(); }

		T const* end() const { return m_buffer.get() +  m_size; }

		T const* data() const { return m_buffer.get(); }

		T* begin() { return m_buffer.get(); }

		T* end() { return m_buffer.get() +  m_size; }

		T* data() { return m_buffer.get(); }

	private:
		std::unique_ptr<T[]> m_buffer;
		size_t m_size;
	};
}

#endif