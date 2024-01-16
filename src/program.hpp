#pragma once

#include "window.hpp"
#include "audio/audio_device.hpp"
#include "audio/sample_config.hpp"
#include "profiling/frame_performance.hpp"


class Program {
    public:
        Program();

        void main_loop();


        /* config */
        // sleep will wake up later than the given time
        // so sleep less and wait the rest out with a spinlock
        static constexpr double SLEEP_REDUCTION = 10.0;  // milliseconds


    private:
        Window main_window;
        WindowData main_window_data;

        double fps_limit;
        FramePerformance frame_perf;

        SampleConfig sample_config;
        AudioPlayback audio_playback;


        /* private functions */
        void handle_sdl_events();
        void update_state();
};
