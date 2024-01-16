#include "exception.hpp"
#include "audio/wave_data.hpp"
#include "audio/audio_funcs.hpp"
#include "audio/sample_config.hpp"

#include <SDL2/SDL.h>

#include <string>
#include <vector>
#include <algorithm>  // copy()
#include <cassert>


namespace AudioFileLoader {

WaveData sdl_wav(const std::string& wav_path, const SampleConfig& sample_config) {
    SDL_AudioSpec wav_spec;
    uint8_t* wav_samples;
    uint32_t wav_size;
    if (SDL_LoadWAV(wav_path.c_str(), &wav_spec, &wav_samples, &wav_size) == NULL)
        throw Exception("Failed to load WAV file\nSDL error: " + std::string(SDL_GetError()));

    SDL_AudioCVT cvt;
    const int ret = SDL_BuildAudioCVT(&cvt, wav_spec.format, wav_spec.channels, wav_spec.freq,
                                      AUDIO_F32SYS, sample_config.n_channels, sample_config.sample_rate);

    if (ret < 0) {
        SDL_FreeWAV(wav_samples);
        throw Exception("Failed to convert WAV to audio device sample specification\nSDL error: " + std::string(SDL_GetError()));
    }
    else if (ret == 0) {
        // no conversion needed
        const float* const f_wav_samples = reinterpret_cast<float*>(wav_samples);
        std::vector<float> samples(f_wav_samples, f_wav_samples + (wav_size / sizeof(float)));
        SDL_FreeWAV(wav_samples);
        return WaveData(std::move(samples), sample_config);
        // return WaveData(std::vector<float>(wav_samples, wav_samples + (wav_size / sizeof(float))), sample_config);
    }
    // else {  // if (ret >= 1) {
        // conversion needed
        cvt.len = wav_size;
        assert(((unsigned int)(cvt.len * cvt.len_mult) >= wav_size) && "Buffer should at least be large enough to hold all samples in WAV file");
        std::vector<float> samples((cvt.len * cvt.len_mult) / sizeof(float));
        const float* const f_wav_samples = reinterpret_cast<float*>(wav_samples);
        std::copy(f_wav_samples, f_wav_samples + (wav_size / sizeof(float)), samples.begin());
        cvt.buf = reinterpret_cast<uint8_t*>(samples.data());

        SDL_ConvertAudio(&cvt);

        // sometimes, `out_size % sample_config.n_channels != 0`
        // round down to be sure we don't read out of bounds
        const int out_size = channel_align_down((cvt.len * cvt.len_ratio) / sizeof(float), sample_config.n_channels);
        samples.resize(out_size);

        SDL_FreeWAV(wav_samples);
        return WaveData(std::move(samples), sample_config);
    // }
}

}  // namespace AudioFileLoader
