//@	{"target":{"name":"video_projector.o"}}

#include "./dispatcher.hpp"
#include "./gl_viewport.hpp"

int main()
{
	auto& ctxt = playback::glfw_context::get();
	playback::gl_viewport viewport{ctxt, 1600, 1000, "video projector"};
	ctxt.read_events([](auto& viewport){
		viewport.swap_buffer();
		return true;
	}, viewport);
}