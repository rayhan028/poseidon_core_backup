#ifndef timer_h_
#define timer_h_

#include <vector>
#include <string>
#include <cassert>
#include <chrono>

#define STEPS

struct timer {
    using time_val = std::chrono::_V2::steady_clock::time_point;

    void start(const std::string &name) {
        if (timing) {
            fprintf(stderr, "\033[0;35mWARNING:\033[0m    Previous timing is not finished\n");
        }
        else {
            starts.push_back({std::chrono::steady_clock::now()});
            names.push_back({name});
            timing = true;
        }
    }

    void stop() {
        if (!timing) {
            fprintf(stderr, "\033[0;35mWARNING:\033[0m    Timer was not started\n");
        }
        else {
            stops.push_back({std::chrono::steady_clock::now()});
            timing = false;
        }
    }

    void print() {
        assert(starts.size() == stops.size());
        for (std::size_t t = 0; t < starts.size(); t++) {
            auto d = std::chrono::duration_cast<std::chrono::microseconds>(stops[t] - starts[t]).count();
            printf("\033[0;32mINFO:\033[0m    %s: %f ms\n", names[t].c_str(), (d / 1000.0));
        }
    }

private:
    std::vector<time_val> starts;
    std::vector<time_val> stops;
    std::vector<std::string> names;
    bool timing = false;
};


#endif