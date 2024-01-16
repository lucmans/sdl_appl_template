#include "profiling/frame_performance.hpp"


FramePerformance::FramePerformance(const int _history_len)
    : history_len(_history_len),
      write_index(0),
      recorded_frame_times(0)
{
    frame_times.resize(history_len, 0.0);
    frame_ready_times.resize(history_len, 0.0);
}


int FramePerformance::get_history_len() const {
    return history_len;
}

// TODO
// void FramePerformance::set_history_len(const int new_history_len) {
//     // copy last `new_history_len` to start of buffers
//     const int copy_n_elements = std::min(new_history_len, recorded_frame_times);

//     std::vector<double> new_frame_times(new_history_len, 0.0);
//     std::vector<double> new_frame_ready_times(new_history_len, 0.0);

//     if (copy_n_elements <= write_index - 1) {
//         std::memcpy(new_frame_times.data(), frame_times.data() + (), copy_n_elements * sizeof(double));
//         std::memcpy(new_frame_ready_times.data(), frame_ready_times.data() + (), copy_n_elements * sizeof(double));
//     }

//     history_len = new_history_len;
// }


void FramePerformance::add_frame_time(const double frame_time, const double frame_ready_time) {
    frame_times[write_index] = frame_time;
    frame_ready_times[write_index] = frame_ready_time;

    write_index = (write_index + 1) % history_len;

    if (recorded_frame_times < history_len)
        recorded_frame_times++;
}


double FramePerformance::get_fps() const {
    if (recorded_frame_times == 0)
        return -1.0;

    double total = 0.0;
    for (int i = 0; i < recorded_frame_times; i++)
        total += frame_times[i];

    return UNIT_PER_SECOND / (total / recorded_frame_times);
}

double FramePerformance::get_unlocked_fps() const {
    if (recorded_frame_times == 0)
        return -1.0;

    double total = 0.0;
    for (int i = 0; i < recorded_frame_times; i++)
        total += frame_ready_times[i];

    return UNIT_PER_SECOND / (total / recorded_frame_times);
}
