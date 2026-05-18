#pragma once

#ifdef ANDROID

#include <string_view>

namespace rpcs3::android
{
	bool initialize(std::string_view user_id);
	int boot_game(std::string_view path);
	void shutdown(bool force_quit);
}

#endif
