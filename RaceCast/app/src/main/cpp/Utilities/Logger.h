//
// Created by brand on 24/2/2026.
//

#ifndef RACECAST_LOGGER_H
#define RACECAST_LOGGER_H

#include <android/log.h>
#define LOG_TAG "LOGGER"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#endif //RACECAST_LOGGER_H
