#include "profiling/startup_timer.hpp"

#include "logger.hpp"

#include "profiling/timer.hpp"


const StartupTimer startup_timer;


StartupTimer::StartupTimer()
    : start_program(Timer::now())
{
    init_time = -1;
}


double StartupTimer::get_program_time() const {
    return Timer::Duration<Timer::ms>(Timer::now() - start_program);
}


double StartupTimer::get_init_time() const {
    if (init_time < 0.0) {
        // TODO: throw exception?
        Logger::warning("Tried to get init time before setting init time");
        return -1;
    }

    return init_time;
}

void StartupTimer::set_init_time() const {
    // only allow being called once
    static bool called = false;
    if (called) {
        Logger::warning("StartupTimer::set_init_time() is called a second time; ignored");
        return;
    }
    called = true;

    init_time = Timer::Duration<Timer::ms>(Timer::now() - start_program);
}
