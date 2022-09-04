//@	{"target":{"name":"video_projector.o"}}

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
	ctxt.read_events([](auto& viewport, auto& eh){
		viewport.swap_buffer();
		return eh.should_exit();
	}, viewport, std::as_const(eh));
}