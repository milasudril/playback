//@	{"target":{"name":"video_projector.o", "dependencies":[{"ref": "GL", "origin":"system", "rel":"external"}]}}

#include "./dispatcher.hpp"
#include "./gl_viewport.hpp"
#include "./mesh.hpp"
#include "./gl_shader.hpp"
#include "./gl_texture.hpp"

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

struct vec3
{
	using value_type = float;

	value_type x;
	value_type y;
	value_type z;
};

constexpr std::array<vec3, 4> vertices{
	vec3{ 0.5f,  0.5f, 0.0f},  // top right
	vec3{ 0.5f, -0.5f, 0.0f},  // bottom right
	vec3{-0.5f, -0.5f, 0.0f},  // bottom left
	vec3{-0.5f,  0.5f, 0.0f}   // top left
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

template<class T>
requires(std::is_trivial_v<T>)
auto load_binary(std::filesystem::path const& path)
{
	std::vector<T> data;
	std::unique_ptr<FILE, decltype(&fclose)> file{fopen(path.c_str(), "rb"), fclose};
	if(file == nullptr)
	{ throw std::runtime_error{"Failed to open input file"}; }

	std::array<T, 4096> buffer;

	while(true)
	{
		auto const n = fread(std::data(buffer), sizeof(T), std::size(buffer), file.get());
		if(n == 0)
		{ return data; }
		std::copy_n(std::begin(buffer), n, std::back_inserter(data));
	}

}

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
	prog.bind();


	playback::gl_vertex_buffer<vec3> vbo;
	vbo.upload(vertices);

	playback::gl_vertex_buffer<playback::uv_coords> uv_buff;
	uv_buff.upload(uvs);

	playback::gl_index_buffer<unsigned int> ibo;
	ibo.upload(faces);
	playback::gl_vertex_array vao;
	vao.set_buffer(0, vbo);
	vao.set_buffer(1, uv_buff);
	vao.set_buffer(ibo);
	vao.bind();

	auto const test_pattern =
		load_binary<pixel_store::rgba_value<>>("/usr/share/test_pattern/test_pattern.rgba");
	playback::gl_texture texture;
	texture.upload(pixel_store::image_span{std::data(test_pattern), 1600, 1000}, 10);
	texture.bind(GL_TEXTURE0);

	ctxt.read_events([](auto& viewport, auto& eh, size_t vertex_count){
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLES, vertex_count, playback::gl_bindings::vertex_index_type(), nullptr);
		viewport.swap_buffer();
		return eh.should_exit();
	}, viewport, std::as_const(eh), std::size(ibo));
}
