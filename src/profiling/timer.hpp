#pragma once

#include <chrono>
#include <ratio>


/* alias to and inherit from std::chrono types to remain compatible with stdlib timing code
 * example usage:
 *   Timer::TimePoint start_time = Timer::now();
 *   // do some work
 *   const double execution_time = Timer::Duration<Timer::ms>(Timer::now() - start_time);
 *   std::cout << "Code took " << execution_time << " milliseconds" << std::endl;
 */
namespace Timer {
    using TimePoint = std::chrono::steady_clock::time_point;
    inline decltype(std::chrono::steady_clock::now)* now = &std::chrono::steady_clock::now;

    using ns = std::nano;
    using us = std::micro;
    using ms = std::milli;
    using sec = std::ratio<1, 1>;
    using min = std::ratio<60, 1>;
    using hours = std::ratio<3600, 1>;

    // `TimeUnit` should one of the aliases above
    template <class TimeUnit>
    class Duration : public std::chrono::duration<double, TimeUnit> {
        public:
            // inherit constructors
            using std::chrono::duration<double, TimeUnit>::duration;

            // implicit conversion to double
            operator double() const {
                return this->count();
            }
    };
};
