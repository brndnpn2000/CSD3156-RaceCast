#ifndef GLOBALS_H
#define GLOBALS_H

#include <utility>

#include <android/log.h>
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "GLOBAL", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "GLOBAL", __VA_ARGS__)

class Globals
{
public:
    inline static std::pair<float, float> screen_size;
    inline static std::pair<int, int> laps;
};


#endif
