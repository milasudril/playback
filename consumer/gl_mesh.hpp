#ifndef PLAYBACK_GLMESH_HPP
#define PLAYBACK_GLMESH_HPP

#include "./gl_buffer.hpp"
#include "./gl_vertex_array.hpp"
#include "./cpu_mesh.hpp"

#include <stdexcept>

namespace playback
{	
	template<class VertexType, class IndexType>
	class gl_mesh
	{
		public:
			static constexpr auto vertex_shader_port = 0;
			static constexpr auto uv_shader_port = 1;
			
			explicit gl_mesh(gl_vertex_buffer<VertexType>&& verts,
				gl_vertex_buffer<uv_coords>&& uvs,
				gl_index_buffer<IndexType>&& faces):
				m_verts{std::move(verts)},
				m_uvs{std::move(uvs)},
				m_faces{std::move(faces)}
			{
				if(std::size(m_verts) != std::size(m_uvs))
				{ throw std::runtime_error{"Mismatch between vertex count and uv point count"}; }
				
				if(std::size(m_faces) % 3 != 0)
				{ throw std::runtime_error{"Bad number of vertex indices"}; }
				
				m_vao.set_buffer(vertex_shader_port, m_verts);
				m_vao.set_buffer(uv_shader_port, m_uvs);
				m_vao.set_buffer(m_faces);
			}
			
			void bind()
			{ m_vao.bind(); }

		private:
			gl_vertex_buffer<VertexType> m_verts;
			gl_vertex_buffer<uv_coords> m_uvs;
			gl_index_buffer<IndexType> m_faces;
			gl_vertex_array m_vao;
	};
}

#endif
