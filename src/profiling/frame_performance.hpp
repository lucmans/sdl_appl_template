#pragma once

#include <vector>


class FramePerformance {
    public:
        /* `history_len` determines the number of data points in ringbuffer for measuring fps
         * the reported fps is the average of all stored frame-times
         */
        FramePerformance(const int _history_len);

        int get_history_len() const;
        // TODO: for now, make a new object
        // // avoid calling often, as frame-time ringbuffers are copied
        // void set_history_len(const int new_history_len);

        // `frame_time` and `frame_ready_time` should be milliseconds
        // otherwise, be sure to update `UNIT_PER_SECOND`
        void add_frame_time(const double frame_time, const double frame_ready_time);

        double get_fps() const;
        double get_unlocked_fps() const;


        /* config */
        // number of the unit supplied to add_frame_time() in a second
        static constexpr double UNIT_PER_SECOND = 1000.0;


    private:
        // maximum number of entries in the ringbuffer
        int history_len;

        // index where a new frame-time given to add_frame_time() should be written to
        int write_index;

        // current number of frame-times; only relevant when ringbuffer is not yet filled
        int recorded_frame_times;

        // ringbuffers containing the frame-times
        std::vector<double> frame_times;
        std::vector<double> frame_ready_times;
};
