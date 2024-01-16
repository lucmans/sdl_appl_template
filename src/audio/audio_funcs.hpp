#pragma once

#include "audio/wave_data.hpp"


// TODO: namespace

// `sample_offset >= 0` most hold
int channel_align(const int sample_offset, const int n_channels);
int channel_align(const double sample_offset, const int n_channels);
int channel_align_up(const int sample_offset, const int n_channels);
int channel_align_up(const double sample_offset, const int n_channels);
int channel_align_down(const int sample_offset, const int n_channels);
int channel_align_down(const double sample_offset, const int n_channels);

void normalize(WaveData& wave_data);
