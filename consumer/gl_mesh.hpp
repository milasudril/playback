#ifndef PLAYBACK_GLMESH_HPP
#define PLAYBACK_GLMESH_HPP

#include "./gl_buffer.hpp"
#include "./gl_vertex_array.hpp"
#include "./mesh_storage.hpp"

#include <stdexcept>

namespace playback
{	
	template<class IndexType>
	class gl_mesh
	{
		public:
			static constexpr auto vertex_shader_port = 0;
			static constexpr auto uv_shader_port = 1;

			template<class T>
			requires(!std::is_same_v<std::decay_t<T>, gl_mesh>)
			explicit gl_mesh(T&& mesh):
				gl_mesh{mesh.vertices(), mesh.uvs(), mesh.faces()}
			{}
			
			template<class PointArray, class UvArray, class FaceArray>
			explicit gl_mesh(PointArray&& verts, UvArray&& uvs, FaceArray&& faces):
				gl_mesh{
					gl_vertex_buffer{std::forward<PointArray>(verts)},
					gl_vertex_buffer{std::forward<UvArray>(uvs)},
					gl_index_buffer{std::forward<FaceArray>(faces)}
				}
			{}

			explicit gl_mesh(gl_vertex_buffer<point>&& verts,
				gl_vertex_buffer<uv_coords>&& uvs,
				gl_index_buffer<IndexType>&& faces):
				m_mesh{std::move(verts), std::move(uvs), std::move(faces)}
			{	
				m_vao.set_buffer(vertex_shader_port, m_mesh.vertices());
				m_vao.set_buffer(uv_shader_port, m_mesh.uvs());
				m_vao.set_buffer(m_mesh.faces());
			}
			
			void bind()
			{ m_vao.bind(); }

		private:
			mesh<gl_vertex_buffer, gl_index_buffer<IndexType>> m_mesh;
			gl_vertex_array m_vao;
	};
}

#endif
