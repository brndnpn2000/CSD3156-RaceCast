//
// Created by brand on 19/2/2026.
//

#include "Timer.h"

Timer::Timer()
{
    m_start_time = std::chrono::high_resolution_clock::now();
    m_last_frame_time = m_start_time;
    m_delta = 0.0f;
    m_elapsed = 0.0f;
}

Timer& Timer::GetInstance()
{
    static Timer instance;
    return instance;
}

void Timer::Update()
{
    auto currentFrameTime = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> delta = currentFrameTime - m_last_frame_time;
    m_delta = delta.count();

    if (m_delta > 0.1f) m_delta = 0.1f;

    std::chrono::duration<float> elapsed = currentFrameTime - m_start_time;
    m_elapsed = elapsed.count();

    m_last_frame_time = currentFrameTime;
}

float Timer::GetDeltaTime()
{
    return m_delta;
}

float Timer::GetElapsedTime()
{
    return m_elapsed;
}

void Timer::Reset()
{
    m_start_time = std::chrono::high_resolution_clock::now();
    m_last_frame_time = m_start_time;
    m_elapsed = 0.0f;
}

