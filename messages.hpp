//@	{"dependencies_extra":[{"ref":"./messages.o", "rel":"implementation"}]}

#ifndef PLAYBACK_MESSAGES_HPP
#define PLAYBACK_MESSAGES_HPP

#include <anon/object.hpp>

#include <cstdint>
#include <array>
#include <vector>
#include <type_traits>

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


	enum class sample_type:int{i8, u8, i16, u16, f32};

	sample_type deserialize(empty<sample_type>, std::string_view str);

	char const* to_string(sample_type type);


	struct video_port_config
	{
		uint32_t width;
		uint32_t height;
		enum sample_type sample_type;
		video_channel_layout channel_layout;
		enum intensity_transfer_function intensity_transfer_function;
		enum alpha_mode alpha_mode;
	};

	video_port_config deserialize(empty<video_port_config>, anon::object const&);

	anon::object serialize(video_port_config const&);



	struct stream_config
	{
		std::vector<video_port_config> video_ports;
	};

	stream_config deserialize(empty<stream_config>, anon::object const&);

	anon::object serialize(stream_config const&);



	struct video_frame_update
	{
		uint32_t video_port;
	};

	video_frame_update deserialize(empty<video_frame_update>, anon::object const&);

	anon::object serialize(video_frame_update const&);
}

#endif