#pragma once

#ifdef ANDROID

#include <string>

struct ANativeWindow;

namespace rpcs3::android
{
	struct window_state
	{
		ANativeWindow* window = nullptr;
		int width = 0;
		int height = 0;
		float refresh_rate = 0.0f;
	};

	void set_paths(std::string executable_dir, std::string config_dir, std::string cache_dir);
	void set_window(ANativeWindow* window, int width, int height, float refresh_rate);
	window_state get_window_state();
	void clear_window();
}

#endif
