//
// Created by saeid on 12/3/20.
//

#ifndef SEPARATOR_PARALLELUTILS_HPP
#define SEPARATOR_PARALLELUTILS_HPP

#include <thread>
#include <vector>
#include <functional>
#include <future>

namespace util {

    void parallel_for(unsigned start, unsigned end, std::function<void(unsigned)> fn, bool par = true) {

        // internal loop
        auto int_fn = [&fn](unsigned int_start, unsigned seg_size) {
            for (unsigned j = int_start; j < int_start + seg_size; j++)
                fn(j);
        };

        // sequenced for
        if (!par)
            return int_fn(start, end);

        // get number of threads
        unsigned nb_threads_hint = std::thread::hardware_concurrency();
        unsigned nb_threads = nb_threads_hint == 0 ? 8 : (nb_threads_hint);

        // calculate segments
        unsigned total_length = end - start;
        unsigned seg = total_length / nb_threads;
        unsigned last_seg = seg + total_length % nb_threads;

        // launch threads - parallel for
        auto fut_vec = std::vector<std::future<void>>();
        fut_vec.reserve(nb_threads);
        for (unsigned k = 0; k < nb_threads - 1; ++k) {
            unsigned current_start = seg * k;
            fut_vec.emplace_back(std::async(int_fn, current_start, seg));
        }
        {
            unsigned current_start = seg * (nb_threads - 1);
            fut_vec.emplace_back(std::async(std::launch::async, int_fn, current_start, last_seg));
        }
        for (auto &th : fut_vec)
            th.get();
    }

}

#endif //SEPARATOR_PARALLELUTILS_HPP
