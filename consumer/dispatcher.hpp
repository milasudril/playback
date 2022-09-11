//@	{"dependencies_extra":[{"ref":"./dispatcher.o", "rel":"implementation"}]}

#ifndef PLAYBACK_DISPATCHER_HPP
#define PLAYBACK_DISPATCHER_HPP

#include "messages.hpp"

#include <anon/object.hpp>

#include <cstdio>
#include <span>

namespace playback
{
	class video_device
	{
	public:
		virtual void process(video_frame_update const&, std::span<std::byte const> data) = 0;
		virtual void process(size_t index, video_port_config const& cfg) = 0;
		virtual void process(std::span<video_port_config const> video_ports) = 0;
	};

	template<class T>
	class video_device_impl : public video_device
	{
	public:
		explicit video_device_impl(T&& dev):m_device{std::move(dev)}{}

		void process(video_frame_update const& msg, std::span<std::byte const> data) override
		{ m_device.process(msg, data); }

		void process(size_t index, video_port_config const& cfg) override
		{ m_device.process(index, cfg); }

		void process(std::span<video_port_config const> video_ports) override
		{ m_device.process(video_ports); }

		T const& get() const { return m_device; }

		T& get() { return m_device; }

	private:
		T m_device;
	};

	struct null_video_device: public video_device
	{
	public:
		void process(video_frame_update const&, std::span<std::byte const>) override { }

		void process(size_t, video_port_config const&) override { }

		void process(std::span<video_port_config const>) override { }
	};

	void dispatch(video_device& video_out, anon::object const& message, std::span<std::byte const> payload);

	template<class VideoDevice>
	void dispatch(VideoDevice&& device, anon::object const& message, std::span<std::byte const> payload)
	{
		video_device_impl dev{std::forward<VideoDevice>(device)};
		dispatch(static_cast<video_device&>(dev), message, payload);
	}

	void dispatch(FILE* src, video_device& video_out);

	template<class VideoDevice>
	void dispatch(FILE* src, VideoDevice&& video_out)
	{
		video_device_impl dev{std::forward<VideoDevice>(video_out)};
		dispatch(src, static_cast<video_device&>(dev));
	}
}

#endif