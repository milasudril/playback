#ifndef PLAYBACK_CPUMESH_HPP
#define PLAYBACK_CPUMESH_HPP

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
		using value_type = vec4_t;
		
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
	
	template<class VertexType, class IndexType>
	class cpu_mesh
	{
	};
}

#endif
