//@	{
//@		"target":{"name":"test_producer.o"}
//@	}

#include "api/message_writer.hpp"
#include "io_utils.hpp"

#include <cmath>

using vec4_t [[gnu::vector_size(16)]] = float;

int main()
{
	setvbuf(stdout, nullptr, _IOFBF, 65536);

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
	cfg.is_slow = false;
	write(cfg, stdout);

	auto test_pattern = playback::load_binary<vec4_t>("/usr/share/test_pattern/test_pattern.rgba");
	auto const test_pattern_span = std::span{test_pattern};

	static constexpr std::chrono::duration<double> delay{1.0/30.0};
	size_t k = 0;
	while(true)
	{
		write_command(playback::video_frame_update{0},
			std::as_bytes(test_pattern_span),
			delay,
			stdout);
		if(k % 30 == 0)
		{
			write_command(playback::set_caption{std::to_string(k/30)}, std::span<std::byte>{}, std::chrono::seconds{0}, stdout);
		}

		std::rotate(std::begin(test_pattern), std::begin(test_pattern) + 6400, std::end(test_pattern));
		++k;
	}
}