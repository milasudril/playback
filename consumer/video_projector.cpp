//@	{"target":{"name":"video_projector.o", "dependencies":[{"ref": "GL", "origin":"system", "rel":"external"}]}}

#include "./dispatcher.hpp"
#include "./gl_viewport.hpp"

namespace
{
	class event_handler
	{
	public:
		event_handler():m_should_exit{false}{}

		bool should_exit() const { return m_should_exit; }

		void window_is_closing() { m_should_exit = true; }

	private:
		bool m_should_exit;
	};
}

int main()
{
	auto& ctxt = playback::glfw_context::get();
	playback::gl_viewport viewport{ctxt, 1600, 1000, "video projector"};
	event_handler eh;
	viewport.set_event_handler(eh);
	viewport.activate_gl_context();
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	ctxt.read_events([](auto& viewport, auto& eh){
		glClear(GL_COLOR_BUFFER_BIT);
		viewport.swap_buffer();
		return eh.should_exit();
	}, viewport, std::as_const(eh));
}