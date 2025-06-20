#pragma once
#include "okami/okami.hpp"

struct FrameTimer
{
    static constexpr size_t MaxSamples = 30;
    std::chrono::steady_clock::time_point LastTime = std::chrono::steady_clock::now();
    std::deque<float> FrameTimesMs;

    void update()
    {
        auto Now = std::chrono::steady_clock::now();
        auto ElapsedMs = std::chrono::duration<float, std::milli>(Now - LastTime).count();

        FrameTimesMs.push_back(ElapsedMs);
        if (FrameTimesMs.size() > MaxSamples)
            FrameTimesMs.pop_front();

        LastTime = Now;
    }

    float getFPS() const
    {
        float Avg = getFrameTimeMs();
        return Avg > 0.0f ? 1000.0f / Avg : 0.0f;
    }

    float getFrameTimeMs() const
    {
        if (FrameTimesMs.empty())
            return 0.0f;
        float Sum = std::accumulate(FrameTimesMs.begin(), FrameTimesMs.end(), 0.0f);
        return Sum / FrameTimesMs.size();
    }
};
