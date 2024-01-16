#include "audio/audio_device.hpp"

#include "logger.hpp"
#include "exception.hpp"
#include "audio/sample_config.hpp"

#include <SDL2/SDL.h>

#include <ostream>
#include <cassert>
#include <string>  // to_string()
#include <vector>
#include <sstream>
#include <algorithm>  // max
#include <ios>  // fixed
#include <iomanip>  // setprecision(), setw()
#include <numeric>  // accumulate()


AudioDevice::AudioDevice(const SampleConfig& _sample_config, const int _frames_per_buffer, const AudioDirection& _audio_direction)
    : audio_direction(_audio_direction),
      sample_config(_sample_config),
      frames_per_buffer(_frames_per_buffer)
{
    SDL_AudioSpec audio_config_want, audio_config_have;
    SDL_memset(&audio_config_want, 0, sizeof(audio_config_want));
    audio_config_want.freq = sample_config.sample_rate;
    audio_config_want.format = AUDIO_F32SYS;
    audio_config_want.channels = sample_config.n_channels;
    audio_config_want.samples = frames_per_buffer;
    audio_config_want.callback = NULL;

    const int sdl_is_capture = static_cast<int>(audio_direction == AudioDirection::capture);
    audio_device = SDL_OpenAudioDevice(NULL, sdl_is_capture, &audio_config_want, &audio_config_have, SDL_AUDIO_ALLOW_ANY_CHANGE);
    if (audio_device == 0)
        throw Exception("Failed to open audio device\nSDL error: " + std::string(SDL_GetError()));

    // check differences
    bool any_different = false;
    if (audio_config_want.freq != audio_config_have.freq) {
        any_different = true;
        Logger::warning("Audio driver cannot provide a sampling rate of '" + std::to_string(audio_config_want.freq) + " Hz'; automatically converting to next best option '" + std::to_string(audio_config_have.freq) + " Hz'");
    }
    if (audio_config_want.format != audio_config_have.format) {
        any_different = true;
        Logger::warning("Audio driver cannot provide sample format '" + std::to_string(audio_config_want.format) + "'; automatically converting to next best option '" + std::to_string(audio_config_have.format) + "'");
    }
    if (audio_config_want.channels != audio_config_have.channels) {
        any_different = true;
        Logger::warning("Audio driver cannot provide the number of channels '" + std::to_string(audio_config_want.channels) + "'; automatically converting to next best option '" + std::to_string(audio_config_have.channels) + "'");
    }
    if (audio_config_want.samples != audio_config_have.samples) {
        any_different = true;
        Logger::warning("Audio driver cannot provide number of frames per buffer '" + std::to_string(audio_config_want.samples) + "'; automatically converting to next best option '" + std::to_string(audio_config_have.samples) + "'");
    }

    // if anything is different, re-open audio device with automatic conversion
    if (any_different) {
        SDL_CloseAudioDevice(audio_device);
        audio_device = SDL_OpenAudioDevice(NULL, sdl_is_capture, &audio_config_want, NULL, 0);
        if (audio_device == 0)
            throw Exception("Failed to open audio device\nSDL error: " + std::string(SDL_GetError()));
    }

    print_audio_settings(std::cout);
}


AudioDevice::~AudioDevice() {
    SDL_CloseAudioDevice(audio_device);
}


AudioDirection AudioDevice::get_audio_direction() const {
    return audio_direction;
}


const SampleConfig& AudioDevice::get_sample_config() const {
    return sample_config;
}


int AudioDevice::get_frames_per_buffer() const {
    return frames_per_buffer;
}


int AudioDevice::get_n_queued_frames() const {
    return (SDL_GetQueuedAudioSize(audio_device) / sizeof(float)) / sample_config.n_channels;
}


int AudioDevice::get_n_queued_samples() const {
    return SDL_GetQueuedAudioSize(audio_device) / sizeof(float);
}


void AudioDevice::clear_queued_samples() {
    SDL_ClearQueuedAudio(audio_device);
}


double AudioDevice::samples_to_ms(const int samples) const {
    // `samples` should always divide by `sample_config.n_channels`
    return (double)(samples / sample_config.n_channels) / sample_config.sample_rate;
}


void AudioDevice::unpause_device() {
    SDL_PauseAudioDevice(audio_device, 0);
}


void AudioDevice::pause_device() {
    SDL_PauseAudioDevice(audio_device, 1);
}


/*static*/ void AudioDevice::print_audio_devices(std::ostream& os, const AudioDirection& audio_direction) {
    const int sdl_is_capture = static_cast<int>(audio_direction == AudioDirection::capture);
    const char* const audio_dir_str = (audio_direction == AudioDirection::capture ? "capture" : "playback");
    const char* const audio_dir_Str = (audio_direction == AudioDirection::capture ? "Capture" : "Playback");
    const int count = SDL_GetNumAudioDevices(sdl_is_capture);
    if (count == -1) {
        Logger::warning("Failed to get list of " + std::string(audio_dir_str) + " devices");
        Logger::hint("The default " + std::string(audio_dir_str) + " device can likely still be opened");
    }
    else {
        os << "--- " << audio_dir_Str << " devices ---\n";
        os << "    Device 0: System default" << "\n";
        for (int i = 0; i < count; i++) {
            const char* const device_name = SDL_GetAudioDeviceName(i, sdl_is_capture);
            os << "    Device " << i + 1 << ": '" << device_name << "'\n";
        }
        os << std::flush;  // TODO: necessary?
    }
}


const char* AudioDevice::get_current_audio_driver() const {
    const char* current_driver = SDL_GetCurrentAudioDriver();
    if (current_driver == NULL)
        throw Exception("No audio driver was initialized");

    return current_driver;
}


/*static*/ void AudioDevice::print_audio_drivers(std::ostream& os) {
    os << "--- Available audio drivers ---\n";
    const int count = SDL_GetNumAudioDrivers();
    for (int i = 0; i < count; i++)
        os << "  * " << SDL_GetAudioDriver(i) << '\n';
    os << std::flush;  // TODO: necessary?
}


void AudioDevice::print_audio_settings(std::ostream& os) const {
    const int precision = 2;
    const double latency = ((double)frames_per_buffer / sample_config.sample_rate) * 1000.0;
    const int buffer_size = frames_per_buffer * sample_config.n_channels * sizeof(float);

    // table headers
    const std::string sample_rate_txt = "Sample rate",
                      format_txt = "Format",
                      channels_txt = "Channels",
                      frames_per_buffer_txt = "Frames per buffer",
                      buffer_size_txt = "Buffer size",
                      driver_latency_txt = "Driver latency (ms)",
                      silence_value_txt = "Silence value";

    const int spacing = 2;
    const std::vector<size_t> column_width = {
        std::max({
            sample_rate_txt.size(),
            format_txt.size(),
            channels_txt.size(),
            frames_per_buffer_txt.size(),
            buffer_size_txt.size(),
            driver_latency_txt.size(),
            silence_value_txt.size(),
        }),
        std::max({
            std::to_string(sample_config.sample_rate).size(),
            std::string("float32").size(),
            std::to_string(sample_config.n_channels).size(),
            std::to_string(frames_per_buffer).size(),
            std::to_string(buffer_size).size(),
            std::to_string((int)latency).size() + 1 + precision,
        }) + spacing,
    };

    // use stringstream to contain `std::fixed`/`std::setprecision` modifiers
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision)
       << "--- Audio " << (audio_direction == AudioDirection::capture ? "capture" : "playback") << " config ---\n"
       << "| " << std::setw(column_width[0]) << "setting"             << std::setw(column_width[1]) << "value"                   << " |\n"
       << "|-" << std::string(std::accumulate(column_width.begin(), column_width.end(), 0), '-')                                 << "-|\n"
       << "| " << std::setw(column_width[0]) << sample_rate_txt       << std::setw(column_width[1]) << sample_config.sample_rate << " |\n"
       << "| " << std::setw(column_width[0]) << format_txt            << std::setw(column_width[1]) << "float32"                 << " |\n"
       << "| " << std::setw(column_width[0]) << channels_txt          << std::setw(column_width[1]) << sample_config.n_channels  << " |\n"
       << "| " << std::setw(column_width[0]) << frames_per_buffer_txt << std::setw(column_width[1]) << frames_per_buffer         << " |\n"
       << "| " << std::setw(column_width[0]) << buffer_size_txt       << std::setw(column_width[1]) << buffer_size               << " |\n"
       << "| " << std::setw(column_width[0]) << driver_latency_txt    << std::setw(column_width[1]) << latency                   << " |\n";

    os << ss.str()
       << std::flush;  // TODO: necessary?
}


/*static*/ void AudioDevice::print_audio_settings(std::ostream& os, const SDL_AudioSpec& want, const SDL_AudioSpec& have, const AudioDirection& audio_direction) {
    const int precision = 2;
    const double want_latency = ((double)want.samples / want.freq) * 1000.0,
                 have_latency = ((double)have.samples / have.freq) * 1000.0;

    // table headers
    const std::string sample_rate_txt = "Sample rate",
                      format_txt = "Format",
                      channels_txt = "Channels",
                      frames_per_buffer_txt = "Frames per buffer",
                      buffer_size_txt = "Buffer size",
                      driver_latency_txt = "Driver latency (ms)",
                      silence_value_txt = "Silence value";

    // calculate width of each column
    const int spacing = 2;
    const std::vector<size_t> column_width = {
        std::max({
            sample_rate_txt.size(),
            format_txt.size(),
            channels_txt.size(),
            frames_per_buffer_txt.size(),
            buffer_size_txt.size(),
            driver_latency_txt.size(),
            silence_value_txt.size(),
        }),
        std::max({
            std::to_string(want.freq).size(),
            std::to_string(want.format).size(),
            std::to_string(want.channels).size(),
            std::to_string(want.samples).size(),
            std::to_string((int)want_latency).size() + 1 + precision,
        }) + spacing,
        std::max({
            std::to_string(have.freq).size(),
            std::to_string(have.format).size(),
            std::to_string(have.channels).size(),
            std::to_string(have.samples).size(),
            std::to_string(have.size).size(),
            std::to_string((int)have_latency).size() + 1 + precision,
            std::to_string(have.silence).size(),
        }) + spacing,
    };

    // use stringstream to contain `std::fixed`/`std::setprecision` modifiers
    std::stringstream ss;

    // cast channels and silence to int, as uint8_t is treated as a character
    ss << std::fixed << std::setprecision(precision)
       << "--- Audio " << (audio_direction == AudioDirection::capture ? "capture" : "playback") << " config ---\n"
       << "| " << std::setw(column_width[0]) << "setting"              << std::setw(column_width[1]) << "want"             << std::setw(column_width[2]) << "have"             << " |\n"
       << "|-" << std::string(std::accumulate(column_width.begin(), column_width.end(), 0), '-')                                                                               << "-|\n"
       << "| " << std::setw(column_width[0]) << sample_rate_txt        << std::setw(column_width[1]) << want.freq          << std::setw(column_width[2]) << have.freq          << " |\n"
       << "| " << std::setw(column_width[0]) << format_txt             << std::setw(column_width[1]) << want.format        << std::setw(column_width[2]) << have.format        << " |\n"
       << "| " << std::setw(column_width[0]) << channels_txt           << std::setw(column_width[1]) << (int)want.channels << std::setw(column_width[2]) << (int)have.channels << " |\n"
       << "| " << std::setw(column_width[0]) << frames_per_buffer_txt  << std::setw(column_width[1]) << want.samples       << std::setw(column_width[2]) << have.samples       << " |\n"
       << "| " << std::setw(column_width[0]) << buffer_size_txt        << std::setw(column_width[1]) << "-"                << std::setw(column_width[2]) << have.size          << " |\n"
       << "| " << std::setw(column_width[0]) << driver_latency_txt     << std::setw(column_width[1]) << want_latency       << std::setw(column_width[2]) << have_latency       << " |\n"
       << "| " << std::setw(column_width[0]) << silence_value_txt      << std::setw(column_width[1]) << "-"                << std::setw(column_width[2]) << (int)have.silence  << " |\n";

    os << ss.str()
       << std::flush;  // TODO: necessary?
}


void AudioPlayback::send_samples(const void* const samples, const int n_samples) {
    if (get_n_queued_samples() == 0)
        Logger::warning("Audio underrun!");

    const int ret = SDL_QueueAudio(audio_device, samples, n_samples * sizeof(float));
    if (ret < 0)
        throw Exception("Failed to queue samples for playback\nSDL error: " + std::string(SDL_GetError()));
}


int AudioCapture::receive_samples(void* const samples, const int n_samples) noexcept {
    const uint32_t bytes_read = SDL_DequeueAudio(audio_device, samples, n_samples * sizeof(float));
    // TODO: repeat until `bytes_read / sizeof(float) < n_samples`? (block?)
    assert((bytes_read % sizeof(float) == 0) && "Read part of a sample");

    return bytes_read / sizeof(float);
}
