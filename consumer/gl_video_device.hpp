#ifndef PLAYBACK_GLVIDEODEVICE_HPP
#define PLAYBACK_GLVIDEODEVICE_HPP

#include "./gl_viewport.hpp"
#include "./gl_video_port.hpp"
#include "./gl_shader.hpp"

#include <algorithm>

namespace playback
{
	inline constexpr const char* vertex_shader_src = R"(#version 450 core

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

	inline constexpr const char* frag_shader_src = R"(#version 450 core
out vec4 FragColor;
in vec2 tex_coord;

layout (location = 0) uniform sampler2D diffuse;

void main()
{
	FragColor = texture(diffuse, tex_coord);
})";


	class gl_video_device:private gl_viewport
	{
	public:
		explicit gl_video_device(glfw_context& ctxt, uint32_t width, uint32_t height):
		gl_viewport{ctxt, width, height, "gl_video_device"},
		m_vert_shader{vertex_shader_src},
		m_frag_shader{frag_shader_src},
		m_shader_prog{m_vert_shader, m_frag_shader}
		{
			glEnable(GL_FRAMEBUFFER_SRGB);
			glEnable(GL_DEPTH_TEST);
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);

			m_shader_prog.bind();
		}

		using gl_viewport::set_event_handler;
		using gl_viewport::activate_gl_context;
		using gl_viewport::swap_buffer;

		void configure_port(size_t index, video_port_config const& cfg)
		{
			 if(index >= std::size(m_video_ports)) [[unlikely]]
			 { m_video_ports.resize(index + 1); }

			 m_video_ports[index] = gl_video_port{cfg};
		}

		void configure(std::span<video_port_config const> video_ports)
		{
			m_video_ports.clear();
			std::ranges::transform(video_ports, std::back_inserter(m_video_ports),
				[](auto const& cfg){
					return gl_video_port(cfg);
			});
		}

		void set_pixels(size_t index, std::span<std::byte const> data)
		{
			if(index >= std::size(m_video_ports)) [[unlikely]]
			{
				fprintf(stderr, "(!) trying to upload pixles to non-existing video port\n");
				return;
			}
			m_video_ports[index].upload_texture(data);
		}

		void render_content() const
		{
			std::ranges::for_each(m_video_ports, [](auto const& item) {
				item.bind();
				glDrawElements(GL_TRIANGLES, item.get_index_count(), playback::gl_bindings::vertex_index_type(), nullptr);
			});
		}

	private:
		gl_shader<GL_VERTEX_SHADER> m_vert_shader;
		gl_shader<GL_FRAGMENT_SHADER> m_frag_shader;
		gl_program m_shader_prog;

		std::vector<gl_video_port> m_video_ports;
	};
};

#endif
