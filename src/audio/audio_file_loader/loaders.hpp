#pragma once

#include "audio/wave_data.hpp"
#include "audio/sample_config.hpp"

#include <string>


namespace AudioFileLoader {

WaveData sdl_wav(const std::string& wav_path, const SampleConfig& sample_config);


inline WaveData best_loader(const std::string& wav_path, const SampleConfig& sample_config) {
    // TODO: FFmpeg loader

    return sdl_wav(wav_path, sample_config);
}

}  // namespace AudioFileLoader
