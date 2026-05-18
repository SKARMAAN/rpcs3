#include "android_application.h"
#include "android_platform.h"

#ifdef ANDROID

#include <android/native_window_jni.h>
#include <jni.h>
#include <string>

namespace
{
	std::string to_string(JNIEnv* env, jstring value)
	{
		if (!value)
		{
			return {};
		}

		const char* chars = env->GetStringUTFChars(value, nullptr);
		std::string result = chars ? chars : "";

		if (chars)
		{
			env->ReleaseStringUTFChars(value, chars);
		}

		return result;
	}
}

extern "C"
{
	JNIEXPORT jboolean JNICALL Java_org_rpcs3_RPCS3Native_initialize(JNIEnv* env, jclass, jstring user_id)
	{
		return rpcs3::android::initialize(to_string(env, user_id)) ? JNI_TRUE : JNI_FALSE;
	}

	JNIEXPORT jint JNICALL Java_org_rpcs3_RPCS3Native_bootGame(JNIEnv* env, jclass, jstring path)
	{
		return static_cast<jint>(rpcs3::android::boot_game(to_string(env, path)));
	}

	JNIEXPORT void JNICALL Java_org_rpcs3_RPCS3Native_shutdown(JNIEnv*, jclass, jboolean force_quit)
	{
		rpcs3::android::shutdown(force_quit == JNI_TRUE);
	}

	JNIEXPORT void JNICALL Java_org_rpcs3_RPCS3Native_setPaths(JNIEnv* env, jclass, jstring executable_dir, jstring config_dir, jstring cache_dir)
	{
		rpcs3::android::set_paths(to_string(env, executable_dir), to_string(env, config_dir), to_string(env, cache_dir));
	}

	JNIEXPORT void JNICALL Java_org_rpcs3_RPCS3Native_setSurface(JNIEnv* env, jclass, jobject surface, jint width, jint height, jfloat refresh_rate)
	{
		ANativeWindow* window = surface ? ANativeWindow_fromSurface(env, surface) : nullptr;
		rpcs3::android::set_window(window, width, height, refresh_rate);
	}

	JNIEXPORT void JNICALL JNI_OnUnload(JavaVM*, void*)
	{
		rpcs3::android::clear_window();
	}
}

#endif
