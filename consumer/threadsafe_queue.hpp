#ifndef PLAYBACK_THREADSAFE_QUEUE_HPP
#define PLAYBACK_THREADSAFE_QUEUE_HPP

#include <queue>
#include <mutex>
#include <shared_mutex>

namespace playback
{
	template<class T>
	class threadsafe_queue
	{
	public:
		void push(T&& obj)
		{
			std::lock_guard lock{m_write_mtx};
			m_queue.push(std::move(obj));
		}

		template<class Callback>
		decltype(auto) with_lock(Callback&& cb)
		{
			std::lock_guard lock{m_write_mtx};
			return cb(m_queue);
		}

	private:
		std::queue<T> m_queue;
		std::mutex m_write_mtx;
	};
}

#endif