#include "program.hpp"

#include "quit.hpp"
#include "logger.hpp"
#include "audio/wave_data.hpp"
#include "audio/audio_file_loader/loaders.hpp"
#include "profiling/frame_performance.hpp"
#include "profiling/timer.hpp"

#include <thread>  // sleep_for()


Program::Program()
    : main_window("Project name", 800, 600, main_window_data),
      frame_perf(20),
      sample_config({.sample_rate=44100, .n_channels=2}),
      audio_playback(sample_config)
{
    fps_limit = 60.0;
}


void Program::main_loop() {
    Timer::TimePoint frame_start = Timer::now();
    Timer::Duration<Timer::ms> frame_time(0.0);
    Timer::Duration<Timer::ms> real_frame_time(0.0);

    audio_playback.unpause_device();
    while (!Quit::poll_quit()) {
        // handle SDL events
        handle_sdl_events();
        if (Quit::poll_quit())
            break;

        // alter internal structures and prepare next frame
        update_state();
        main_window.prepare_frame(main_window_data);

        // calculate real frame rate
        real_frame_time = Timer::now() - frame_start;

        // sleep rest of frame out
        const double milliseconds_in_frame = 1000.0 / fps_limit;
        std::this_thread::sleep_for(Timer::Duration<Timer::ms>((milliseconds_in_frame - real_frame_time) - SLEEP_REDUCTION));
        // spin lock rest of time if `SLEEP_REDUCTION` is used
        while (Timer::Duration<Timer::ms>(Timer::now() - frame_start) < milliseconds_in_frame);
        // calculate frame rate
        frame_time = Timer::now() - frame_start;
        frame_start = Timer::now();
        frame_perf.add_frame_time(frame_time, real_frame_time);

        // render frame
        main_window.render_frame();
    }
}


void Program::handle_sdl_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e) && !Quit::poll_quit()) {
        switch (e.type) {
            case SDL_QUIT:
            case SDL_APP_TERMINATING:
                Quit::set_quit();
                break;  // TODO: return?

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_q:
                    case SDLK_ESCAPE:
                        Quit::set_quit();
                        break;  // TODO: return?

                    // DEBUG: stop audio playback
                    case SDLK_s:
                        audio_playback.clear_queued_samples();
                        break;
                }
                break;

            // case SDL_KEYUP:
            //     break;

            // case SDL_MOUSEBUTTONDOWN:
            //     handle_click(e.button.x, e.button.y, e.button.button);
            //     break;

            // case SDL_MOUSEBUTTONUP:
            //     break;

            // case SDL_MOUSEWHEEL:
            //     if (e.wheel.y > 0) {
            //         // scroll up
            //     }
            //     else if (e.wheel.y < 0) {
            //         // scroll down
            //     }

            //     if (e.wheel.x > 0) {
            //         // scroll right
            //     }
            //     else if (e.wheel.x < 0) {
            //         // scroll left
            //     }
            //     break;

            // case SDL_MOUSEMOTION:
            //     break;

            case SDL_DROPFILE:
                {
                    const std::string dropped_file_path(e.drop.file);
                    SDL_free(e.drop.file);
                    WaveData song = AudioFileLoader::best_loader(dropped_file_path, sample_config);
                    audio_playback.clear_queued_samples();
                    audio_playback.send_samples(song.samples.data(), song.samples.size());
                    break;
                }

            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        Quit::set_quit();
                        break;

                    // case SDL_WINDOWEVENT_SIZE_CHANGED:  // all size changes
                    case SDL_WINDOWEVENT_RESIZED:  // only final size
                        main_window.set_resolution(e.window.data1, e.window.data2, main_window_data);
                        // TODO: force redraw?
                        break;

                    // case SDL_WINDOWEVENT_ENTER:
                    //     break;

                    // case SDL_WINDOWEVENT_LEAVE:
                    //     break;
                }
                break;

            case SDL_RENDER_TARGETS_RESET:
            case SDL_RENDER_DEVICE_RESET:
                Logger::error("Graphics had a mishap");
                break;
        }
    }
}


void Program::update_state() {
    main_window_data.fps_data.fps = frame_perf.get_fps();
}
