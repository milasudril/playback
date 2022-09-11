//@	{"target":{"name":"dispatcher.test"}}

#include "./dispatcher.hpp"

#include "testfwk/testfwk.hpp"

namespace
{
	struct dummy_video_device
	{
		void process(playback::video_frame_update const& msg, std::span<std::byte const> buff)
		{
			video_frame_update.get() = msg;
			payload.get() = buff;
		}

		template<class ... T>
		void process(T const& ...) {abort();}


		std::reference_wrapper<std::span<std::byte const>> payload;
		std::reference_wrapper<playback::video_frame_update> video_frame_update;

	};
}

TESTCASE(dispatch_video_frame_update)
{
	std::array<std::byte, 4> payload{};
	playback::video_frame_update content{};
	content.video_port = 1;

	anon::object msg;
	msg.insert_or_assign("message_type", "video_frame_update")
		.insert_or_assign("payload_size", std::size(payload))
		.insert_or_assign("content", serialize(content));

	std::span<std::byte const> payload_out;
	playback::video_frame_update vfu_out;

	playback::dispatch(dummy_video_device{payload_out, vfu_out}, msg, payload);

	EXPECT_EQ(vfu_out.video_port, content.video_port);
	EXPECT_EQ(std::data(payload_out), std::data(payload));
	EXPECT_EQ(std::size(payload_out), std::size(payload));
}