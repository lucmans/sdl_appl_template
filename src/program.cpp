#include "program.hpp"

#include "rsc_dir.hpp"
#include "quit.hpp"
#include "logger.hpp"
#include "audio/wav_file.hpp"
#include "profiling/frame_performance.hpp"

#include <chrono>
#include <thread>  // sleep_for()


Program::Program()
    : main_window("Project name", 800, 600),
      frame_perf(120),
      sample_config({.sample_rate=44100, .sample_format=AUDIO_F32, .n_channels=2, .samples_per_buffer=512}),
      audio_playback(sample_config)
{
    fps_limit = 60.0;
}


void Program::main_loop() {
    std::chrono::steady_clock::time_point frame_start = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> frame_time = std::chrono::duration<double>(0.0);
    std::chrono::duration<double, std::milli> real_frame_time = std::chrono::duration<double>(0.0);

    while (!Quit::poll_quit()) {
        // handle SDL events
        handle_sdl_events();
        if (Quit::poll_quit())
            break;

        // alter internal structures and prepare next frame
        update_state();
        main_window.prepare_frame(main_window_data);

        // calculate real frame rate
        real_frame_time = std::chrono::steady_clock::now() - frame_start;

        // sleep rest of frame out
        const double milliseconds_in_frame = 1000.0 / fps_limit;
        std::this_thread::sleep_for(std::chrono::duration<double, std::milli>((milliseconds_in_frame - real_frame_time.count()) - SLEEP_REDUCTION));
        // spin lock rest of time
        while (std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - frame_start).count() < milliseconds_in_frame);
        // calculate frame rate
        frame_time = std::chrono::steady_clock::now() - frame_start;
        frame_start = std::chrono::steady_clock::now();
        frame_perf.add_frame_time(frame_time.count(), real_frame_time.count());

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
                break;

            case SDL_KEYDOWN:
                switch (e.key.keysym.sym) {
                    case SDLK_q:
                    case SDLK_ESCAPE:
                        Quit::set_quit();
                        break;

                    // DEBUG: stop audio playback
                    case SDLK_s:
                        audio_playback.clear_queued_samples();
                        break;

                    // DEBUG: play some .wav file
                    case SDLK_a:
                        WavFile song(RscDir::get() / "song.wav", sample_config);
                        audio_playback.send_samples(song.get_samples().data(), song.get_n_samples());
                        audio_playback.unpause_device();
                        break;
                }
                break;

            // case SDL_KEYUP:
            //     break;

            // case SDL_MOUSEBUTTONDOWN:
            //     break;

            // case SDL_MOUSEBUTTONUP:
            //     break;

            // case SDL_MOUSEWHEEL:
            //     break;

            // case SDL_MOUSEMOTION:
            //     break;

            // case SDL_DROPFILE:
            //     deal_with(std::string(e.drop.file));
            //     SDL_free(e.drop.file);
            //     break;

            case SDL_WINDOWEVENT:
                switch (e.window.event) {
                    case SDL_WINDOWEVENT_CLOSE:
                        Quit::set_quit();
                        break;

                    // case SDL_WINDOWEVENT_SIZE_CHANGED:  // all size changes
                    case SDL_WINDOWEVENT_RESIZED:  // only final size
                        main_window.set_resolution(e.window.data1, e.window.data2);
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
