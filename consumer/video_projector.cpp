//@	{"target":{"name":"video_projector.o", "dependencies":[{"ref": "GL", "origin":"system", "rel":"external"}]}}

#include "./dispatcher.hpp"
#include "./gl_video_device.hpp"
#include "./gl_shader.hpp"
#include "io_utils.hpp"
#include "./nonblocking_fd.hpp"

#include <GL/gl.h>

#include <filesystem>
#include <concepts>
#include <type_traits>
#include <anon/deserializer.hpp>

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

constexpr std::array<playback::point, 4> vertices{
	playback::point{ 0.5f,  0.5f, 0.0f},  // top right
	playback::point{ 0.5f, -0.5f, 0.0f},  // bottom right
	playback::point{-0.5f, -0.5f, 0.0f},  // bottom left
	playback::point{-0.5f,  0.5f, 0.0f}   // top left
};

constexpr std::array<playback::uv_coords, 4> uvs{
	playback::uv_coords{ 1.0f,  0.0f},
	playback::uv_coords{ 1.0f,  1.0f},
	playback::uv_coords{ 0.0f,  1.0f},
	playback::uv_coords{ 0.0f,  0.0f}
};

constexpr std::array<unsigned int, 6> faces{
	0, 3, 1,
	3, 2, 1
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


	video_out.configure_port(0, playback::video_port_config{
		.width = 1600,
		.height = 1000,
		.channel_layout = playback::video_channel_layout::rgba,
		.sample_type = playback::sample_type::f32,
		.intensity_transfer_function = playback::intensity_transfer_function::linear,
		.alpha_mode = playback::alpha_mode::premultiplied,
		.num_mipmaps = 0
	});

	video_out.set_pixels(0, playback::load_binary<std::byte>("/usr/share/test_pattern/test_pattern.rgba"));


	ctxt.read_events([reader = playback::nonblocking_fd{STDIN_FILENO},
		parse_ctxt = anon::create_parser_context()
	](auto& video_out, auto& eh){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		video_out.render_content();
		video_out.swap_buffer();
		return eh.should_exit();
	}, video_out, std::as_const(eh));
}
