add_library(rpcs3_android SHARED
    android_application.cpp
    android_gs_frame.cpp
    android_jni.cpp
    android_platform.cpp
)

target_link_libraries(rpcs3_android
    PRIVATE
        rpcs3_emu
        android
        log
)
