#pragma once

#include "profiling/timer.hpp"


// TODO: make static instead of global?
class StartupTimer {
    public:
        StartupTimer();

        /* all get_*_time() functions return number of milliseconds */
        double get_program_time() const;  // time since program start
        double get_init_time() const;  // program init time
        void set_init_time() const;  // only called once before main loop


    private:
        const Timer::TimePoint start_program;

        // mutable, so the global object can be const
        // set_init_time() ensures it can only be set once
        mutable double init_time;  // milliseconds
};

extern const StartupTimer startup_timer;
