#pragma once

#ifdef ANDROID

#include "Emu/RSX/GSFrameBase.h"

#include <atomic>
#include <vector>

class android_gs_frame final : public GSFrameBase
{
public:
	android_gs_frame() = default;
	~android_gs_frame() override = default;

	void close() override;
	void reset() override;
	bool shown() override;
	void hide() override;
	void show() override;
	void toggle_fullscreen() override;

	void delete_context(draw_context_t ctx) override;
	draw_context_t make_context() override;
	void set_current(draw_context_t ctx) override;
	void flip(draw_context_t ctx, bool skip_frame = false) override;
	int client_width() override;
	int client_height() override;
	f64 client_display_rate() override;
	bool has_alpha() override;

	display_handle_t handle() const override;

	bool can_consume_frame() const override;
	void present_frame(std::vector<u8>&& data, u32 pitch, u32 width, u32 height, bool is_bgra) const override;
	void take_screenshot(std::vector<u8>&& sshot_data, u32 sshot_width, u32 sshot_height, bool is_bgra) override;

private:
	std::atomic<bool> m_visible{true};
};

#endif
