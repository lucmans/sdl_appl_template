#include "audio/audio_device.hpp"

#include "logger.hpp"
#include "exception.hpp"

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


AudioDevice::AudioDevice(const SampleConfig& sample_config, const AudioDirection& _audio_direction)
    : audio_direction(_audio_direction),
      bytes_per_sample(SDL_AUDIO_BITSIZE(sample_config.sample_format) / 8)
{
    SDL_memset(&audio_config_want, 0, sizeof(audio_config_want));
    audio_config_want.freq = sample_config.sample_rate;
    audio_config_want.format = sample_config.sample_format;
    audio_config_want.channels = sample_config.n_channels;
    audio_config_want.samples = sample_config.samples_per_buffer;
    audio_config_want.callback = NULL;

    const int sdl_is_capture = static_cast<int>(audio_direction == AudioDirection::capture);
    const int allow_change = (ALLOW_AUTOMATIC_CONVERSION ? SDL_AUDIO_ALLOW_ANY_CHANGE : 0);
    audio_device = SDL_OpenAudioDevice(NULL, sdl_is_capture, &audio_config_want, &audio_config_have, allow_change);
    if (audio_device == 0)
        throw Exception("Failed to open audio device\nSDL error: " + std::string(SDL_GetError()));
}


AudioDevice::~AudioDevice() {
    SDL_CloseAudioDevice(audio_device);
}


AudioDirection AudioDevice::get_audio_direction() const {
    return audio_direction;
}


int AudioDevice::get_n_queued_samples() const {
    return SDL_GetQueuedAudioSize(audio_device) / bytes_per_sample;
}


void AudioDevice::clear_queued_samples() {
    SDL_ClearQueuedAudio(audio_device);
}


int AudioDevice::get_sample_size() const {
    return bytes_per_sample;
}


double AudioDevice::samples_to_ms(const int samples) const {
    return (double)samples / (double)audio_config_have.freq;
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


/*static*/ void AudioDevice::print_audio_settings(std::ostream& os, const SDL_AudioSpec& want, const SDL_AudioSpec& have, const AudioDirection& audio_direction) {
    const int precision = 2;
    const double want_latency = ((double)want.samples / (double)want.freq) * 1000.0,
                 have_latency = ((double)have.samples / (double)have.freq) * 1000.0;

    // table headers
    const std::string sample_rate = "Sample rate",
                      format = "Format",
                      channels = "Channels",
                      samples_per_buffer = "Samples per buffer",
                      buffer_size = "Buffer size",
                      driver_latency = "Driver latency (ms)",
                      silence_value = "Silence value";

    // calculate width of each column
    const int spacing = 2;
    const std::vector<size_t> column_width = {
        std::max({
            sample_rate.size(),
            format.size(),
            channels.size(),
            samples_per_buffer.size(),
            buffer_size.size(),
            driver_latency.size(),
            silence_value.size()
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
            std::to_string(have.silence).size()
        }) + spacing
    };

    // use stringstream to contain `std::fixed`/`std::setprecision` modifiers
    std::stringstream ss;

    // cast channels and silence to int, as uint8_t is treated as a character
    ss << std::fixed << std::setprecision(precision)
       << "--- Audio " << (audio_direction == AudioDirection::capture ? "capture" : "playback") << " config ---\n"
       << "| " << std::setw(column_width[0]) << "setting"          << std::setw(column_width[1]) << "want"             << std::setw(column_width[2]) << "have"             << " |\n"
       << "|-" << std::string(std::accumulate(column_width.begin(), column_width.end(), 0), '-')                                                                      << "-|\n"
       << "| " << std::setw(column_width[0]) << sample_rate        << std::setw(column_width[1]) << want.freq          << std::setw(column_width[2]) << have.freq          << " |\n"
       << "| " << std::setw(column_width[0]) << format             << std::setw(column_width[1]) << want.format        << std::setw(column_width[2]) << have.format        << " |\n"
       << "| " << std::setw(column_width[0]) << channels           << std::setw(column_width[1]) << (int)want.channels << std::setw(column_width[2]) << (int)have.channels << " |\n"
       << "| " << std::setw(column_width[0]) << samples_per_buffer << std::setw(column_width[1]) << want.samples       << std::setw(column_width[2]) << have.samples       << " |\n"
    //    << "| " << std::setw(column_width[0]) << buffer_size        << std::setw(column_width[1]) << "-"                << std::setw(column_width[2]) << have.size          << " |\n"
       << "| " << std::setw(column_width[0]) << driver_latency     << std::setw(column_width[1]) << want_latency       << std::setw(column_width[2]) << have_latency       << " |\n"
       << "| " << std::setw(column_width[0]) << silence_value      << std::setw(column_width[1]) << "-"                << std::setw(column_width[2]) << (int)have.silence  << " |\n";

    os << ss.str()
       << std::flush;  // TODO: necessary?
}


void AudioDevice::print_audio_settings(std::ostream& os) const {
    AudioDevice::print_audio_settings(os, audio_config_want, audio_config_have, audio_direction);
}


void AudioPlayback::send_samples(const void* const samples, const int n_samples) {
    const int ret = SDL_QueueAudio(audio_device, samples, n_samples * bytes_per_sample);
    if (ret < 0)
        throw Exception("Failed to queue samples for playback\nSDL error: " + std::string(SDL_GetError()));
}


int AudioCapture::receive_samples(void* const samples, const int n_samples) noexcept {
    const uint32_t bytes_read = SDL_DequeueAudio(audio_device, samples, n_samples * bytes_per_sample);
    assert((bytes_read % bytes_per_sample == 0) && "Read part of a sample");

    return bytes_read / bytes_per_sample;
}
