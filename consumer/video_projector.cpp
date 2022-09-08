//@	{"target":{"name":"video_projector.o", "dependencies":[{"ref": "GL", "origin":"system", "rel":"external"}]}}

#include "./dispatcher.hpp"
#include "./gl_viewport.hpp"
#include "./gl_mesh.hpp"
#include "./gl_shader.hpp"
#include "./gl_texture.hpp"
#include "./gl_video_port.hpp"

#include <GL/gl.h>

#include <filesystem>
#include <concepts>
#include <type_traits>

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

constexpr const char* vertex_shader_src = R"(#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uv;
out vec2 tex_coord;

void main()
{
	gl_Position = vec4(aPos, 1.0);
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
	playback::gl_viewport viewport{ctxt, 800, 500, "video projector"};
	event_handler eh;
	viewport.set_event_handler(eh);
	viewport.activate_gl_context();

	fprintf(stderr, "(i) Initialized OpenGL reporting version %s\n", glGetString(GL_VERSION));

	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	playback::gl_shader<GL_VERTEX_SHADER> vertex_shader{vertex_shader_src};
	playback::gl_shader<GL_FRAGMENT_SHADER> frag_shader{frag_shader_src};
	playback::gl_program prog{vertex_shader, frag_shader};
	prog.bind();
	
	playback::gl_video_port video_port{};
	video_port.bind();

	ctxt.read_events([](auto& viewport, auto& eh){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, 6, playback::gl_bindings::vertex_index_type(), nullptr);
		viewport.swap_buffer();
		return eh.should_exit();
	}, viewport, std::as_const(eh));
}
