#include "android_application.h"

#ifdef ANDROID

#include "android_gs_frame.h"

#include "Emu/System.h"
#include "Emu/system_config.h"
#include "Emu/RSX/Null/NullGSRender.h"
#ifdef HAVE_VULKAN
#include "Emu/RSX/VK/VKGSRender.h"
#endif
#include "Emu/Io/Null/NullKeyboardHandler.h"
#include "Emu/Io/Null/NullMouseHandler.h"
#include "Emu/Io/Null/null_camera_handler.h"
#include "Emu/Io/Null/null_music_handler.h"
#include "Emu/Audio/Null/NullAudioBackend.h"
#include "Emu/Audio/Null/null_enumerator.h"
#include "Emu/Audio/Cubeb/CubebBackend.h"
#include "Emu/Audio/Cubeb/cubeb_enumerator.h"
#include "Input/pad_thread.h"
#include "Utilities/Thread.h"
#include "util/atomic.hpp"
#include "util/logs.hpp"

#include <optional>

LOG_CHANNEL(android_log, "ANDROID");

namespace rpcs3::android
{
	namespace
	{
		void set_callbacks()
		{
			EmuCallbacks callbacks{};

			callbacks.call_from_main_thread = [](std::function<void()> func, atomic_t<u32>* wake_up)
			{
				func();

				if (wake_up)
				{
					*wake_up = true;
					wake_up->notify_one();
				}
			};

			callbacks.on_run = [](bool) {};
			callbacks.on_pause = []() {};
			callbacks.on_resume = []() {};
			callbacks.on_stop = []() {};
			callbacks.on_ready = []() {};
			callbacks.on_missing_fw = []() {};
			callbacks.on_emulation_stop_no_response = [](std::shared_ptr<atomic_t<bool>>, int) {};
			callbacks.on_save_state_progress = [](std::shared_ptr<atomic_t<bool>>, stx::shared_ptr<utils::serial>, stx::atomic_ptr<std::string>*, std::shared_ptr<void>) {};
			callbacks.enable_disc_eject = [](bool) {};
			callbacks.enable_disc_insert = [](bool) {};
			callbacks.try_to_quit = [](bool, std::function<void()> on_exit) -> bool
			{
				if (on_exit)
				{
					on_exit();
				}

				return true;
			};
			callbacks.handle_taskbar_progress = [](s32, s32) {};

			callbacks.init_kb_handler = []()
			{
				ensure(g_fxo->init<KeyboardHandlerBase, NullKeyboardHandler>(Emu.DeserialManager()));
			};
			callbacks.init_mouse_handler = []()
			{
				ensure(g_fxo->init<MouseHandlerBase, NullMouseHandler>(Emu.DeserialManager()));
			};
			callbacks.init_pad_handler = [](std::string_view title_id)
			{
				ensure(g_fxo->init<named_thread<pad_thread>>(nullptr, nullptr, title_id));
			};

			callbacks.update_emu_settings = []() {};
			callbacks.save_emu_settings = []() {};

			callbacks.close_gs_frame = []() {};
			callbacks.get_gs_frame = []() -> std::unique_ptr<GSFrameBase>
			{
				return std::make_unique<android_gs_frame>();
			};

			callbacks.get_camera_handler = []() -> std::shared_ptr<camera_handler_base>
			{
				return std::make_shared<null_camera_handler>();
			};

			callbacks.get_music_handler = []() -> std::shared_ptr<music_handler_base>
			{
				return std::make_shared<null_music_handler>();
			};

			callbacks.init_gs_render = [](utils::serial* ar)
			{
				switch (g_cfg.video.renderer.get())
				{
				case video_renderer::null:
				{
					g_fxo->init<rsx::thread, named_thread<NullGSRender>>(ar);
					break;
				}
				case video_renderer::vulkan:
				{
#ifdef HAVE_VULKAN
					g_fxo->init<rsx::thread, named_thread<VKGSRender>>(ar);
#else
					android_log.error("Vulkan renderer is not available in this build, falling back to Null renderer.");
					g_fxo->init<rsx::thread, named_thread<NullGSRender>>(ar);
#endif
					break;
				}
				case video_renderer::opengl:
				{
					android_log.error("OpenGL renderer is not available on Android, falling back to Null renderer.");
					g_fxo->init<rsx::thread, named_thread<NullGSRender>>(ar);
					break;
				}
				}
			};

			callbacks.get_audio = []() -> std::shared_ptr<AudioBackend>
			{
				std::shared_ptr<AudioBackend> result;
				switch (g_cfg.audio.renderer.get())
				{
				case audio_renderer::null:
					result = std::make_shared<NullAudioBackend>();
					break;
				case audio_renderer::cubeb:
					result = std::make_shared<CubebBackend>();
					break;
				default:
					result = std::make_shared<NullAudioBackend>();
					break;
				}

				if (!result->Initialized())
				{
					android_log.error("Audio renderer %s could not be initialized, using a Null renderer instead.", result->GetName());
					result = std::make_shared<NullAudioBackend>();
				}
				return result;
			};

			callbacks.get_audio_enumerator = [](u64 renderer) -> std::shared_ptr<audio_device_enumerator>
			{
				switch (static_cast<audio_renderer>(renderer))
				{
				case audio_renderer::null:
					return std::make_shared<null_enumerator>();
				case audio_renderer::cubeb:
					return std::make_shared<cubeb_enumerator>();
				default:
					return std::make_shared<null_enumerator>();
				}
			};

			callbacks.get_msg_dialog = []() -> std::shared_ptr<MsgDialogBase> { return {}; };
			callbacks.get_osk_dialog = []() -> std::shared_ptr<OskDialogBase> { return {}; };
			callbacks.get_save_dialog = []() -> std::unique_ptr<SaveDialogBase> { return {}; };
			callbacks.get_sendmessage_dialog = []() -> std::shared_ptr<SendMessageDialogBase> { return {}; };
			callbacks.get_recvmessage_dialog = []() -> std::shared_ptr<RecvMessageDialogBase> { return {}; };
			callbacks.get_trophy_notification_dialog = []() -> std::unique_ptr<TrophyNotificationBase> { return {}; };

			callbacks.get_localized_string = [](localized_string_id, const char*) -> std::string { return {}; };
			callbacks.get_localized_u32string = [](localized_string_id, const char*) -> std::u32string { return {}; };
			callbacks.get_localized_setting = [](const cfg::_base*, u32) -> std::string { return {}; };
			callbacks.get_photo_path = [](std::string_view) { return std::string{}; };
			callbacks.play_sound = [](const std::string&, std::optional<f32>) {};
			callbacks.get_image_info = [](const std::string&, std::string&, s32&, s32&, s32&) { return false; };
			callbacks.get_scaled_image = [](const std::string&, s32, s32, s32&, s32&, u8*, bool) { return false; };
			callbacks.resolve_path = [](std::string_view arg) { return std::string{arg}; };
			callbacks.get_font_dirs = []() { return std::vector<std::string>{}; };
			callbacks.on_install_pkgs = [](const std::vector<std::string>&) { return false; };
			callbacks.add_breakpoint = [](u32) {};
			callbacks.display_sleep_control_supported = []() { return false; };
			callbacks.enable_display_sleep = [](bool) {};
			callbacks.check_microphone_permissions = []() {};
			callbacks.make_video_source = []() -> std::unique_ptr<video_source> { return {}; };
			callbacks.enable_gamemode = [](bool) {};
			callbacks.get_database_config = [](const std::string&) { return std::string{}; };

			Emu.SetCallbacks(std::move(callbacks));
		}
	}

	bool initialize(std::string_view user_id)
	{
		const std::string resolved_user = user_id.empty() ? "00000001" : std::string(user_id);

		Emu.SetHasGui(true);
		Emu.SetUsr(resolved_user);
		Emu.Init();

		set_callbacks();
		return true;
	}

	int boot_game(std::string_view path)
	{
		if (path.empty())
		{
			return static_cast<int>(game_boot_result::nothing_to_boot);
		}

		return static_cast<int>(Emu.BootGame(std::string(path)));
	}

	void shutdown(bool force_quit)
	{
		if (force_quit)
		{
			Emu.Kill();
			return;
		}

		Emu.GracefulShutdown();
	}
}

#endif
