#pragma once

#include "audio/sample_config.hpp"

#include <SDL2/SDL.h>

#include <ostream>


static_assert(sizeof(float) == 4, "Floats are expected to be 4 bytes");


enum class AudioDirection {
    playback,
    capture
};


// assumes samples are a whole number (non fraction) of bytes
class AudioDevice {
    // only allow specific audio devices (playback/capture) to be instantiated
    protected:
        // throws exception on failure
        AudioDevice(const SampleConfig& _sample_config, const int _frames_per_buffer, const AudioDirection& _audio_direction);

        AudioDevice(const AudioDevice&) = delete;
        AudioDevice& operator=(const AudioDevice&) = delete;


    public:
        virtual ~AudioDevice();

        AudioDirection get_audio_direction() const;

        const SampleConfig& get_sample_config() const;
        int get_frames_per_buffer() const;

        int get_n_queued_frames() const;
        int get_n_queued_samples() const;
        void clear_queued_samples();

        // if `samples % sample_config.n_channels != 0`, it is rounded down to the lower multiple of `sample_config.n_channels`
        double samples_to_ms(const int samples) const;

        // starts/stops gathering/playing samples
        void unpause_device();
        void pause_device();

        static void print_audio_devices(std::ostream& os, const AudioDirection& audio_direction);

        // returned string is owned by SDL; don't free it!
        const char* get_current_audio_driver() const;
        static void print_audio_drivers(std::ostream& os);

        void print_audio_settings(std::ostream& os) const;
        // TODO: pretty print sample format
        static void print_audio_settings(std::ostream& os, const SDL_AudioSpec& want, const SDL_AudioSpec& have, const AudioDirection& audio_direction);


    protected:
        SDL_AudioDeviceID audio_device;
        const AudioDirection audio_direction;

        const SampleConfig sample_config;
        const int frames_per_buffer;
};


class AudioPlayback : public AudioDevice {
    public:
        AudioPlayback(const SampleConfig& _sample_config, const int _frames_per_buffer = 512)
            : AudioDevice(_sample_config, _frames_per_buffer, AudioDirection::playback) {};

        // throws exception on failure
        // basic exception guarantee
        void send_samples(const void* const samples, const int n_samples);
};


class AudioCapture : public AudioDevice {
    public:
        AudioCapture(const SampleConfig& _sample_config, const int _frames_per_buffer = 512)
            : AudioDevice(_sample_config, _frames_per_buffer, AudioDirection::capture) {};

        // returns number of received samples
        int receive_samples(void* const samples, const int n_samples) noexcept;
};
