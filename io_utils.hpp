#ifndef PLAYBACK_IOUTILS_HPP
#define PLAYBACK_IOUTILS_HPP

#include <filesystem>
#include <memory>
#include <cstdio>

namespace playback
{
	template<class T>
	requires(std::is_trivial_v<T>)
	auto load_binary(std::filesystem::path const& path)
	{
		std::vector<T> data;
		std::unique_ptr<FILE, decltype(&fclose)> file{fopen(path.c_str(), "rb"), fclose};

		if(file == nullptr)
		{ throw std::runtime_error{"Failed to open input file"}; }

		std::array<T, 4096> buffer;

		while(true)
		{
			auto const n = fread(std::data(buffer), sizeof(T), std::size(buffer), file.get());
			if(n == 0)
			{ return data; }
			std::copy_n(std::begin(buffer), n, std::back_inserter(data));
		}
	}
}

#endif
