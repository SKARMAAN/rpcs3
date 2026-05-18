#include "android_gs_frame.h"

#ifdef ANDROID

#include "android_platform.h"

#include <android/native_window.h>

void android_gs_frame::close()
{
	m_visible = false;
}

void android_gs_frame::reset()
{
}

bool android_gs_frame::shown()
{
	return m_visible.load();
}

void android_gs_frame::hide()
{
	m_visible = false;
}

void android_gs_frame::show()
{
	m_visible = true;
}

void android_gs_frame::toggle_fullscreen()
{
}

void android_gs_frame::delete_context(draw_context_t)
{
}

draw_context_t android_gs_frame::make_context()
{
	return nullptr;
}

void android_gs_frame::set_current(draw_context_t)
{
}

void android_gs_frame::flip(draw_context_t, bool)
{
}

int android_gs_frame::client_width()
{
	const auto state = rpcs3::android::get_window_state();
	if (state.width > 0)
	{
		return state.width;
	}

	return state.window ? ANativeWindow_getWidth(state.window) : 0;
}

int android_gs_frame::client_height()
{
	const auto state = rpcs3::android::get_window_state();
	if (state.height > 0)
	{
		return state.height;
	}

	return state.window ? ANativeWindow_getHeight(state.window) : 0;
}

f64 android_gs_frame::client_display_rate()
{
	const auto state = rpcs3::android::get_window_state();
	return state.refresh_rate > 0.0f ? state.refresh_rate : 60.0;
}

bool android_gs_frame::has_alpha()
{
	return false;
}

display_handle_t android_gs_frame::handle() const
{
	const auto state = rpcs3::android::get_window_state();
	return display_handle_t{state.window};
}

bool android_gs_frame::can_consume_frame() const
{
	return rpcs3::android::get_window_state().window != nullptr;
}

void android_gs_frame::present_frame(std::vector<u8>&&, u32, u32, u32, bool) const
{
}

void android_gs_frame::take_screenshot(std::vector<u8>&&, u32, u32, bool)
{
}

#endif
