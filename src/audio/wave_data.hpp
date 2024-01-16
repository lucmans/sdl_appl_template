#pragma once

#include "exception.hpp"
#include "audio/sample_config.hpp"

#include <vector>
#include <limits>


/* floating point sample data
 * channels must be interleaved
 * there must be at least 1 sample per channel and at most `INT_MAX - overflow_headroom` samples
 * as there cannot be more than INT_MAX samples, `samples.size()` can safely be converted to `int`
 */
struct WaveData {
    const SampleConfig sample_config;
    std::vector<float> samples;


    WaveData(const SampleConfig& _sample_config)
        : sample_config(_sample_config) {}

    // WaveData(float* data, const int size, const SampleConfig& _sample_config)
    //     : sample_config(_sample_config), samples(std::make_move_iterator(data), std::make_move_iterator(data + size)) {}

    static constexpr int max_n_samples = std::numeric_limits<int>::max();

    WaveData(std::vector<float>&& data, const SampleConfig& _sample_config)
        : sample_config(_sample_config), samples(std::move(data))
    {
        if (samples.size() == 0)
            throw Exception("Empty audio data");

        if (samples.size() > max_n_samples)
            throw Exception("Audio data is too long  (" + std::to_string(samples.size()) + " > " + std::to_string(max_n_samples) + ")");

        if (samples.size() % sample_config.n_channels != 0)
            throw Exception("Not all channels are equal in length");
    }
};
