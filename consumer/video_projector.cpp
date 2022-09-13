//@	{
//@		"target":{"name":"video_projector.o",
//@		"dependencies":[
//@			{"ref": "GL", "origin":"system", "rel":"external"},
//@			{"ref": "anon", "origin":"system", "rel":"external"}
//@		]}
//@	}

#include "./dispatcher.hpp"
#include "./gl_video_device.hpp"
#include "./gl_shader.hpp"
#include "io_utils.hpp"
#include "./nonblocking_fd.hpp"
#include "./fd_reader.hpp"

#include <anon/deserializer.hpp>

#include <GL/gl.h>

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

constexpr const char* vertex_shader_src = R"(#version 450 core

layout (location = 0) in vec4 input_point;
layout (location = 1) in vec2 uv;
layout (location = 1) uniform vec2 window_scale;
layout (location = 3) uniform vec3 model_scale;
out vec2 tex_coord;

vec4 origin = vec4(0, 0, 0, 1);

void main()
{
	vec4 scale = vec4(window_scale, 1, 0)  * vec4(model_scale, 0);
	gl_Position = scale * (input_point - origin) + origin;
	tex_coord = uv;
})";

constexpr const char* frag_shader_src = R"(#version 450 core
out vec4 FragColor;
in vec2 tex_coord;

layout (location = 0) uniform sampler2D diffuse;

void main()
{
	FragColor = texture(diffuse, tex_coord);
})";

struct command_reader
{
};

int main()
{
	auto& ctxt = playback::glfw_context::get();
	playback::gl_video_device video_out{ctxt, 800, 500};
	event_handler eh;
	video_out.set_event_handler(eh);
	video_out.activate_gl_context();

	fprintf(stderr, "(i) Initialized OpenGL reporting version %s\n", glGetString(GL_VERSION));

	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	playback::gl_shader<GL_VERTEX_SHADER> vertex_shader{vertex_shader_src};
	playback::gl_shader<GL_FRAGMENT_SHADER> frag_shader{frag_shader_src};
	playback::gl_program prog{vertex_shader, frag_shader};
	prog.bind();
	eh.framebuffer_size_changed(800, 500);

	playback::fd_reader reader{STDIN_FILENO};
	auto const stream_cfg = deserialize(playback::empty<playback::stream_config>{}, anon::load(reader));
	video_out.configure(stream_cfg.video_ports);

	playback::nonblocking_fd unnblock_stdin{STDIN_FILENO};

	ctxt.read_events([anon_reader = anon::async_loader{reader}] (auto& video_out, auto& eh) mutable {
		if(auto res = anon_reader.try_read_next<anon::object>(); res.has_value())
		{
			puts("Hej");
		}

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		video_out.render_content();
		video_out.swap_buffer();
		return eh.should_exit();
	}, video_out, std::as_const(eh));
}
