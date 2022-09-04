//@	{"dependencies_extra":[{"ref":"./dispatcher.o", "rel":"implementation"}]}

#ifndef PLAYBACK_DISPATCHER_HPP
#define PLAYBACK_DISPATCHER_HPP

#include "./device.hpp"

#include <anon/object.hpp>
#include <cstdio>

namespace playback
{
	void dispatch(FILE* src, device& dev, anon::object const& message);

	void dispatch(FILE* src, device& dev);
}

#endif