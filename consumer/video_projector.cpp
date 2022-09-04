//@	{"target":{"name":"video_projector.o", "dependencies":[{"ref": "GL", "origin":"system", "rel":"external"}]}}

#include "./dispatcher.hpp"
#include "./gl_viewport.hpp"
#include "./gl_buffer.hpp"
#include "./gl_shader.hpp"
#include "./gl_vertex_array.hpp"

#include <GL/gl.h>

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
out vec3 vertexPos;
out vec4 vertexColor;

void main()
{
	gl_Position = vec4(aPos, 1.0);
	vertexColor = 0.16*vec4(1, 0.25, 0.5, 1.0);
	vertexPos = aPos;
})";

constexpr const char* frag_shader_src = R"(#version 450 core
out vec4 FragColor;
in vec4 vertexColor;
in vec3 vertexPos;
layout (location = 1) uniform vec2 lightpos;

void main()
{
	vec3 d = vec3(lightpos, 0.25) - vertexPos;
	float distance = sqrt( dot(d, d) );
	vec4 light = vec4(1, 1, 1, 1);
	FragColor = vertexColor*light/distance;
})";

constexpr std::array<float, 18> vertices{
    // first triangle
     0.5f,  0.5f, 0.0f,  // top right
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f,  0.5f, 0.0f,  // top left
    // second triangle
     0.5f, -0.5f, 0.0f,  // bottom right
    -0.5f, -0.5f, 0.0f,  // bottom left
    -0.5f,  0.5f, 0.0f   // top left
};

int main()
{
	auto& ctxt = playback::glfw_context::get();
	playback::gl_viewport viewport{ctxt, 1600, 1000, "video projector"};
	event_handler eh;
	viewport.set_event_handler(eh);
	viewport.activate_gl_context();

	fprintf(stderr, "(i) Initialized OpenGL reporting version %s\n", glGetString(GL_VERSION));

	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

	playback::gl_shader<GL_VERTEX_SHADER> vertex_shader{vertex_shader_src};
	playback::gl_shader<GL_FRAGMENT_SHADER> frag_shader{frag_shader_src};
	playback::gl_program prog{vertex_shader, frag_shader};
	prog.use();

	playback::gl_buffer vbo;
	vbo.upload(std::as_bytes(std::span{std::begin(vertices), std::end(vertices)}));
	playback::gl_vertex_array vao;
	vao.set_vertex_buffer(0, vbo, 3*sizeof(float));
	vbo.bind_to_array_buffer();
	vao.bind();


	ctxt.read_events([](auto& viewport, auto& eh){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		viewport.swap_buffer();
		return eh.should_exit();
	}, viewport, std::as_const(eh));
}