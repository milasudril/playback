#ifndef PLAYBACK_MESSAGEDISPATCHER_HPP
#define PLAYBACK_MESSAGEDISPATCHER_HPP

#include <anon/object.hpp>

#include <span>
#include <string_view>

class message_dispatcher
{
public:
	void dispatch(std::string_view message_type,
		anon::object const& content,
		std::span<std::byte const> payload);

private:

};

#endif