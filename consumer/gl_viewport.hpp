//@	{
//@	 "dependencies":[{"ref": "glfw3", "origin":"pkg-config"}]
//@	}

#ifndef PLAYBACK_GLVIEWPORT_HPP
#define PLAYBACK_GLVIEWPORT_HPP

#include <GLFW/glfw3.h>

#include <memory>

namespace playback
{
	struct window_handle_deleter
	{
		void operator()(GLFWwindow* window) const
		{
			if(window != nullptr)
			{
				glfwDestroyWindow(window);
			}
		}
	};

	using window_handle = std::unique_ptr<GLFWwindow, window_handle_deleter>;

	class glfw_context
	{
	public:
		static glfw_context& get()
		{
			static glfw_context ctxt;
			return ctxt;
		}

		template<class Callable, class ... Args>
		void read_events(Callable&& f, Args&&... args)
		{
			while(f(args...))
			{
				glfwPollEvents();
			}
		}

	private:
		glfw_context()
		{
			glfwInit();
		}

		~glfw_context()
		{
			glfwTerminate();
		}
	};

	class gl_viewport
	{
	public:
		explicit gl_viewport(glfw_context const&,
			uint32_t width, uint32_t height, char const* title):
			m_window{glfwCreateWindow(width, height, title, nullptr, nullptr)}
		{}

		void swap_buffer()
		{
			glfwSwapBuffers(m_window.get());
		}

	private:
		window_handle m_window;
	};
}

#endif