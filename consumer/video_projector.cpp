//@	{
//@		"target":{"name":"video_projector.o"}
//@	}

#include "./gl_video_device.hpp"
#include "./nonblocking_fd.hpp"
#include "./fd_reader.hpp"
#include "./command_reader.hpp"

#include "io_utils.hpp"

#include <GL/gl.h>
#include <thread>

namespace
{
	class event_handler
	{
	public:
		event_handler():m_should_exit{false}{}

		bool should_exit() const { return m_should_exit; }

		void window_is_closing() { m_should_exit = true; }

		void framebuffer_size_changed(int w, int h)
		{
			glViewport(0,0, w, h);
			if( w > h)
			{
				auto const ratio = static_cast<float>(h)/static_cast<float>(w);
				glUniform2f(1, ratio, 1.0f);
			}
			else
			{
				auto const ratio = static_cast<float>(w) / static_cast<float>(h);
				glUniform2f(1, 1.0f, ratio);
			}
		}

	private:
		bool m_should_exit;
	};
}

int main()
{
	auto& ctxt = playback::glfw_context::get();
	playback::gl_video_device video_out{ctxt, 800, 500};
	event_handler eh;
	video_out.set_event_handler(eh);

	eh.framebuffer_size_changed(800, 500);

	playback::fd_reader reader{STDIN_FILENO};
	auto const stream_cfg = deserialize(playback::empty<playback::stream_config>{}, anon::load(reader));
	video_out.configure(stream_cfg.video_ports);

	auto unnblock_stdin = stream_cfg.is_slow?
		std::optional<playback::nonblocking_fd>{STDIN_FILENO}:
		std::optional<playback::nonblocking_fd>{};

	playback::message_dispatcher dispatcher{video_out};

	std::jthread reader_thread{[&reader, &dispatcher, &eh]() {
		playback::command_reader cmd_reader{reader, dispatcher};
		while(!eh.should_exit())
		{
			cmd_reader.read_and_dispatch();
		}
	}};

	ctxt.read_events([&dispatcher]
		(auto& video_out, auto& eh) mutable {
		dispatcher.flush_expired_commands(std::chrono::steady_clock::now());
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		video_out.render_content();
		video_out.swap_buffer();
		if(eh.should_exit())
		{
			dispatcher.set_should_terminate();
			return true;
		}
		return false;
	}, video_out, std::as_const(eh));
}
