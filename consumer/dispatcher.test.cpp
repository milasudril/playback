//@	{"target":{"name":"dispatcher.test"}}

#include "./dispatcher.hpp"

#include "testfwk/testfwk.hpp"

namespace
{
	struct dummy_device:public playback::device
	{
		void process(playback::video_frame_update const& msg, std::span<std::byte> buff) override
		{
			video_frame_update = msg;
			payload = buff;
		}

		void process(playback::stream_config const&) override {abort();}

		std::span<std::byte> payload{};

		playback::video_frame_update video_frame_update{};

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

	dummy_device dev;
	dispatch(dev, msg, payload);

	EXPECT_EQ(dev.video_frame_update.video_port, content.video_port);
	EXPECT_EQ(std::data(dev.payload), std::data(payload));
	EXPECT_EQ(std::size(dev.payload), std::size(payload));
}