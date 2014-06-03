/*
 * Copyright (C) 1998-2014 ALPS Collaboration. See COPYRIGHT.TXT
 * All rights reserved. Use is subject to license terms. See LICENSE.TXT
 * For use in publications, see ACKNOWLEDGE.TXT
 */

#ifndef ALPS_NGS_SCHEDULER_CHECK_SCHEDULE_HPP
#define ALPS_NGS_SCHEDULER_CHECK_SCHEDULE_HPP

#include <boost/chrono.hpp>

namespace alps {
    
    class check_schedule
    {
    public:
        typedef boost::chrono::high_resolution_clock clock;
        typedef clock::time_point time_point;
        typedef boost::chrono::duration<double> duration;

        check_schedule(double tmin=60., double tmax=900.)
        :   min_check_(tmin)
        ,   max_check_(tmax)
        {
        }

        bool pending() const
        {
            time_point now = clock::now();
            return now > (last_check_time_ + next_check_);
        }
    
        double check_interval() const
        {
            return next_check_.count();
        }

        void update(double fraction)
        {
            time_point now = clock::now();

            // first check
            if( start_time_ == time_point() )
            {
                start_time_ = now;
                start_fraction_ = fraction;
            }

            if( fraction > start_fraction_ )
            {
                // estimate remaining time; propose to run 1/4 of that time
                duration old_check = next_check_;
                next_check_ = 0.25 * (1 - fraction) * (now - start_time_) / (fraction - start_fraction_);
                if( next_check_ > 2*old_check ) next_check_ = 2 * old_check;
                if( next_check_ < min_check_ ) next_check_ = min_check_;
                if( next_check_ > max_check_ ) next_check_ = max_check_;
            }
            else
                next_check_ = min_check_;

            last_check_time_ = now;
        }

    private:
        duration min_check_;
        duration max_check_;

        time_point start_time_;
        time_point last_check_time_;
        double start_fraction_;
        duration next_check_;
    
    };

} // namespace alps 

#endif // !defined ALPS_NGS_SCHEDULER_CHECK_SCHEDULE_HPP
