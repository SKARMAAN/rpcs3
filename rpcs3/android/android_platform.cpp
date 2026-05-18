#include "android_platform.h"

#ifdef ANDROID

#include "Utilities/File.h"

#include <android/native_window.h>
#include <mutex>

namespace rpcs3::android
{
	namespace
	{
		std::mutex g_window_mutex;
		ANativeWindow* g_window = nullptr;
		int g_window_width = 0;
		int g_window_height = 0;
		float g_window_refresh_rate = 0.0f;
	}

	void set_paths(std::string executable_dir, std::string config_dir, std::string cache_dir)
	{
		fs::set_android_paths(std::move(executable_dir), std::move(config_dir), std::move(cache_dir));
	}

	void set_window(ANativeWindow* window, int width, int height, float refresh_rate)
	{
		ANativeWindow* acquired_window = window;
		if (acquired_window)
		{
			ANativeWindow_acquire(acquired_window);
		}

		std::lock_guard lock(g_window_mutex);

		if (g_window == window)
		{
			if (acquired_window)
			{
				ANativeWindow_release(acquired_window);
			}

			g_window_width = width;
			g_window_height = height;
			g_window_refresh_rate = refresh_rate;
			return;
		}

		if (g_window)
		{
			ANativeWindow_release(g_window);
		}

		g_window = acquired_window;

		g_window_width = width;
		g_window_height = height;
		g_window_refresh_rate = refresh_rate;
	}

	window_state get_window_state()
	{
		std::lock_guard lock(g_window_mutex);
		return {g_window, g_window_width, g_window_height, g_window_refresh_rate};
	}

	void clear_window()
	{
		std::lock_guard lock(g_window_mutex);

		if (g_window)
		{
			ANativeWindow_release(g_window);
		}

		g_window = nullptr;
		g_window_width = 0;
		g_window_height = 0;
		g_window_refresh_rate = 0.0f;
	}
}

#endif
