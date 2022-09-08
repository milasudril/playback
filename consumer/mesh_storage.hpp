#ifndef PLAYBACK_MESHSTORAGE_HPP
#define PLAYBACK_MESHSTORAGE_HPP

namespace playback
{
	struct uv_coords
	{
		using value_type = float;
		float u;
		float v;
	};
	
	using vec4_t [[gnu::vector_size(16)]] = float;
	
	class point
	{
	public:
		using value_type = float;
		
		constexpr point() = default; // so we can load from file and set w afterwards
		
		constexpr explicit point(float x, float y, float z): m_value{x, y, z, 1.0f}
		{}
		
		constexpr vec4_t value() const
		{ return m_value; }
		
		constexpr void set_w()
		{ m_value[3] = 1.0f; }
		
	private:
		vec4_t m_value;
	};
	
	constexpr auto origin()
	{
		return point{0.0f, 0.0f, 0.0f};
	}
	
	template<template<class> class VertexBufferType, class IndexBufferType>
	class mesh
	{
	public:
		using point_array = VertexBufferType<point>;
		using uv_array = VertexBufferType<uv_coords>;
		using face_array = IndexBufferType;

		constexpr explicit mesh(point_array&& verts,
			uv_array&& uvs,
			face_array&& faces):
			m_verts{std::move(verts)},
			m_uvs{std::move(uvs)},
			m_faces{std::move(faces)}
		{
			if(std::size(m_verts) != std::size(m_uvs))
			{ throw std::runtime_error{"Mismatch between vertex count and uv point count"}; }
			
			if(std::size(m_faces) % 3 != 0)
			{ throw std::runtime_error{"Bad number of vertex indices"}; }
		}		
	
		constexpr auto const& vertices() const
		{ return m_verts; }
		
		constexpr auto const& uvs() const
		{ return m_uvs; }
		
		constexpr auto const& faces() const
		{ return m_faces; }

	private:
		point_array m_verts;
		uv_array m_uvs;
		face_array m_faces;
	};
}

#endif
