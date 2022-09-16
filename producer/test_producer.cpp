//@	{
//@		"target":{"name":"test_producer.o"}
//@	}

#include "./message_writer.hpp"
#include "io_utils.hpp"

int main()
{
	playback::stream_config cfg;
	cfg.video_ports.push_back(playback::video_port_config{
		.width = 1600,
		.height = 1000,
		.channel_layout = playback::video_channel_layout::rgba,
		.sample_type = playback::sample_type::f32,
		.intensity_transfer_function = playback::intensity_transfer_function::linear,
		.alpha_mode = playback::alpha_mode::premultiplied,
		.num_mipmaps = 10
	});
	write(cfg, stdout);

	auto test_pattern = playback::load_binary<std::byte>("/usr/share/test_pattern/test_pattern.rgba");
	write_message(playback::video_frame_update{0}, test_pattern, stdout);
/*
 	video_ports:obj*{
		width:u32{1600\}
		height:u32{1000\}
		channel_layout:str{rgba\}
		intensity_transfer_function:str{linear\}
		sample_type:str{f32\}
		alpha_mode:str{premultiplied\}
		num_mipmaps:u32{10\}\;
	\}*/
}