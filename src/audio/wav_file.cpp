#include "audio/wav_file.hpp"

#include "exception.hpp"

#include <SDL2/SDL.h>

#include <string>
#include <vector>
#include <algorithm>  // copy()


WavFile::WavFile(const std::string& _wav_path, const SampleConfig& _sample_config)
        : wav_path(_wav_path),
          sample_config(_sample_config)
{
    SDL_AudioSpec wav_spec;
    uint8_t* wav_samples;
    uint32_t wav_size;
    if (SDL_LoadWAV(wav_path.c_str(), &wav_spec, &wav_samples, &wav_size) == NULL)
        throw Exception("Failed to load WAV file\nSDL error: " + std::string(SDL_GetError()));

    SDL_AudioCVT cvt;
    const int ret = SDL_BuildAudioCVT(&cvt, wav_spec.format, wav_spec.channels, wav_spec.freq,
                                      sample_config.sample_format, sample_config.n_channels, sample_config.sample_rate);

    if (ret <= -1) {
        SDL_FreeWAV(wav_samples);
        throw Exception("Failed to convert WAV to audio device sample specification\nSDL error: " + std::string(SDL_GetError()));
    }
    else if (ret == 0) {
        // no conversion needed
        samples = std::vector<uint8_t>(wav_samples, wav_samples + wav_size);
    }
    else {  // if (ret >= 1) {
        // conversion needed
        cvt.len = wav_size;
        samples = std::vector<uint8_t>(cvt.len * cvt.len_mult);
        std::copy(wav_samples, wav_samples + wav_size, samples.begin());
        cvt.buf = samples.data();

        SDL_ConvertAudio(&cvt);

        samples.resize(cvt.len * cvt.len_ratio);
    }

    SDL_FreeWAV(wav_samples);
}


const std::vector<uint8_t>& WavFile::get_samples() const {
    return samples;
}


int WavFile::get_n_samples() const {
    return samples.size() / (SDL_AUDIO_BITSIZE(sample_config.sample_format) / 8);
}
