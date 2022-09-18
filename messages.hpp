//@	{"dependencies_extra":[{"ref":"./messages.o", "rel":"implementation"}]}

#ifndef PLAYBACK_MESSAGES_HPP
#define PLAYBACK_MESSAGES_HPP

#include "./uninitialized_buffer.hpp"

#include <anon/object.hpp>

#include <cstdint>
#include <array>
#include <vector>
#include <type_traits>
#include <chrono>

namespace playback
{
	template<class T>
	using empty = std::type_identity<T>;

	template<class T>
	T deserialize(empty<T>, std::string const& str)
	{
		return deserialize(empty<T>{}, std::string_view{str});
	}

	enum class intensity_transfer_function:int{linear};

	intensity_transfer_function deserialize(empty<intensity_transfer_function>,
		std::string_view str);

	char const* to_string(intensity_transfer_function val);



	enum class alpha_mode:int{premultiplied, straight};

	alpha_mode deserialize(empty<alpha_mode>, std::string_view str);

	char const* to_string(alpha_mode val);



	enum class video_channel_layout:int{y, ya, rgb, rgba};

	video_channel_layout deserialize(empty<video_channel_layout>, std::string_view str);

	char const* to_string(video_channel_layout val);

	inline size_t get_channel_count(video_channel_layout layout)
	{
		// HACK: Correct for now, see above
		return static_cast<size_t>(layout) + 1;
	}



	enum class sample_type:int{i8, u8, i16, u16, f32};

	sample_type deserialize(empty<sample_type>, std::string_view str);

	char const* to_string(sample_type type);

	inline size_t  get_sample_size(sample_type type)
	{
		// HACK: Correct for now, see above
		std::array<size_t, 5> sizes{1, 1, 2, 2, 4};

		return sizes[static_cast<size_t>(type)];
	}



	struct video_port_config
	{
		uint32_t width;
		uint32_t height;
		video_channel_layout channel_layout;
		enum sample_type sample_type;
		enum intensity_transfer_function intensity_transfer_function;
		enum alpha_mode alpha_mode;
		uint32_t num_mipmaps;
	};

	inline size_t get_pixel_count(video_port_config const& cfg)
	{
		return static_cast<size_t>(cfg.width) * static_cast<size_t>(cfg.height);
	}

	inline size_t get_pixel_size(video_port_config const& cfg)
	{
		return get_channel_count(cfg.channel_layout) * get_sample_size(cfg.sample_type);
	}

	video_port_config deserialize(empty<video_port_config>, anon::object const&);

	anon::object serialize(video_port_config const&);



	struct stream_config
	{
		std::vector<video_port_config> video_ports;
		bool is_slow;
	};

	stream_config deserialize(empty<stream_config>, anon::object const&);

	anon::object serialize(stream_config const&);



	struct video_frame_update
	{
		static constexpr char const* message_type_name = "video_frame_update";
		uint32_t video_port;
	};

	video_frame_update deserialize(empty<video_frame_update>, anon::object const&);

	anon::object serialize(video_frame_update const&);



	struct command
	{
		std::string message_type_name;
		anon::object content;
		std::chrono::steady_clock::duration delay;
		uninitialized_buffer<std::byte> payload;
	};
}

#endif