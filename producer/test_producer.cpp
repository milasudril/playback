//@	{
//@		"target":{"name":"test_producer.o"}
//@	}

#include "./message_writer.hpp"
#include "io_utils.hpp"

#include <cmath>

using vec4_t [[gnu::vector_size(16)]] = float;

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

	auto test_pattern = playback::load_binary<vec4_t>("/usr/share/test_pattern/test_pattern.rgba");
	auto const test_pattern_span = std::span{test_pattern};

	while(true)
	{
		write_message(playback::video_frame_update{0}, std::as_bytes(test_pattern_span), stdout);
		std::rotate(std::begin(test_pattern), std::begin(test_pattern) + 3200, std::end(test_pattern));
	}
}