//
// Created by brand on 19/2/2026.
//

#ifndef RACECAST_TIMER_H
#define RACECAST_TIMER_H

#include <chrono>

class Timer
{
public:
    Timer();
    static Timer& GetInstance();

    void Update();
    float GetDeltaTime();
    float GetElapsedTime();
    void Reset();

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_last_frame_time;
    float m_delta;
    float m_elapsed;
};

#define TIMER Timer::GetInstance()

#endif //RACECAST_TIMER_H
