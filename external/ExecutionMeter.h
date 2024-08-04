#pragma once
//Author: Ataa A. Elganayni.
//First Version: January 2020.
//**************************************

#include <chrono>

namespace Hats::Tools
{
    template <class T=std::milli>
    class Meter
    {
    private:
        std::chrono::time_point<std::chrono::steady_clock> m_start;

    public:
        Meter() : m_start(std::chrono::high_resolution_clock::now())
        {

        }
        ~Meter() = default;

        double ElapsedTime()
        {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, T> m_elapsed = end - m_start;

            return (m_elapsed.count());
        }
    };
}
