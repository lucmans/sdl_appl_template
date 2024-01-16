#pragma once

#include <SDL2/SDL.h>

#include <ostream>


enum class AudioDirection {
    playback,
    capture
};


struct SampleConfig {
    const int sample_rate;
    const SDL_AudioFormat sample_format;
    const int n_channels;
    const int samples_per_buffer;
};


// assumes samples are a whole number (non fraction) of bytes
class AudioDevice {
    // only allow specific audio devices (playback/capture) to be instantiated
    protected:
        // throws exception on failure
        AudioDevice(const SampleConfig& sample_config, const AudioDirection& _audio_direction);

        AudioDevice(const AudioDevice&) = delete;
        AudioDevice& operator=(const AudioDevice&) = delete;


    public:
        virtual ~AudioDevice();

        AudioDirection get_audio_direction() const;

        int get_n_queued_samples() const;
        void clear_queued_samples();

        int get_sample_size() const;

        double samples_to_ms(const int samples) const;

        // starts/stops gathering/playing samples
        void unpause_device();
        void pause_device();

        static void print_audio_devices(std::ostream& os, const AudioDirection& audio_direction);

        // returned string is owned by SDL; don't free it!
        const char* get_current_audio_driver() const;
        static void print_audio_drivers(std::ostream& os);

        // TODO: pretty print sample format
        static void print_audio_settings(std::ostream& os, const SDL_AudioSpec& want, const SDL_AudioSpec& have, const AudioDirection& audio_direction);
        void print_audio_settings(std::ostream& os) const;


    protected:
        /* config */
        // allow automatic sample config conversion if device doesn't accept given sample config
        const bool ALLOW_AUTOMATIC_CONVERSION = true;


        /* member variables */
        SDL_AudioDeviceID audio_device;
        const AudioDirection audio_direction;
        // these are considered const outside of the constructor
        /*const*/ SDL_AudioSpec audio_config_want, audio_config_have;

        const int bytes_per_sample;
};


class AudioPlayback : public AudioDevice {
    public:
        AudioPlayback(const SampleConfig& sample_config)
            : AudioDevice(sample_config, AudioDirection::playback) {};

        // throws exception on failure
        // basic exception guarantee
        void send_samples(const void* const samples, const int n_samples);
};


class AudioCapture : public AudioDevice {
    public:
        AudioCapture(const SampleConfig& sample_config)
            : AudioDevice(sample_config, AudioDirection::capture) {};

        // returns number of received samples
        int receive_samples(void* const samples, const int n_samples) noexcept;
};
