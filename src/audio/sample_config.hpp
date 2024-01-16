#pragma once

#include <SDL2/SDL.h>


// TODO: own sample format specifier instead of SDL
struct SampleConfig {
    const int sample_rate;
    const int n_channels;
};
