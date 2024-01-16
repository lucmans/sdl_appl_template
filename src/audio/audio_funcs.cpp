#include "audio_funcs.hpp"

#include "audio/wave_data.hpp"

#include <cmath>


int channel_align(const int sample_offset, const int n_channels) {
    const int channel_misalignment = sample_offset % n_channels;

    return (sample_offset - channel_misalignment) + (
        // if alignment is closer to the last aligning sample
        channel_misalignment < n_channels / 2
        // then: round down
        ? 0
        // else: round up
        : n_channels
    );
}


int channel_align(const double sample_offset, const int n_channels) {
    // decimal number of samples from last channel-aligned sample
    const double channel_misalignment = std::fmod(sample_offset, (double)n_channels);

    return (int)std::round(sample_offset - channel_misalignment) + (
        // if alignment is closer to the last aligning sample
        channel_misalignment < (double)n_channels / 2.0
        // then: round down
        ? 0
        // else: round up
        : n_channels
    );
}


int channel_align_up(const int sample_offset, const int n_channels) {
    const int channel_misalignment = sample_offset % n_channels;

    if (channel_misalignment == 0)
        return sample_offset;
    else
        return (sample_offset - channel_misalignment) + n_channels;
}


int channel_align_up(const double sample_offset, const int n_channels) {
    // decimal number of samples from last channel-aligned sample
    const double channel_misalignment = std::fmod(sample_offset, (double)n_channels);

    if (channel_misalignment == 0.0)
        return sample_offset;
    else
        return (int)std::round(sample_offset - channel_misalignment) + n_channels;
}


int channel_align_down(const int sample_offset, const int n_channels) {
    const int channel_misalignment = sample_offset % n_channels;

    return sample_offset - channel_misalignment;
}


int channel_align_down(const double sample_offset, const int n_channels) {
    // decimal number of samples from last channel-aligned sample
    const double channel_misalignment = std::fmod(sample_offset, (double)n_channels);

    return (int)std::round(sample_offset - channel_misalignment);
}


void normalize(WaveData& wave_data) {
    float max_value = 0.0;
    for (const float sample : wave_data.samples) {
        if (std::abs(sample) > max_value)
            max_value = sample;
    }

    const float factor = 1.0f / max_value;
    for (float& sample : wave_data.samples)
        sample *= factor;
}
