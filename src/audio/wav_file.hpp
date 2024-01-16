#pragma once

#include "audio_device.hpp"  // SampleConfig

#include <string>
#include <vector>


// loads whole wav file into memory
class WavFile {
    public:
        // throws exception if acquiring samples fails
        WavFile(const std::string& _wav_path, const SampleConfig& _sample_config);
        // TODO: move constructor/assignment

        const std::vector<uint8_t>& get_samples() const;
        int get_n_samples() const;


    private:
        const std::string wav_path;
        const SampleConfig sample_config;
        std::vector<uint8_t> samples;
};
