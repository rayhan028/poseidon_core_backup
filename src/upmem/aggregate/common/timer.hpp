#ifndef timer_h_
#define timer_h_

#include <vector>
#include <string>
#include <cassert>
#include <chrono>

#define STEPS

#if 1

using time_val = std::chrono::_V2::steady_clock::time_point;

struct event {
    std::vector<time_val> starts;
    std::vector<time_val> stops;
    std::string name;
};

struct timer {
    void start(const std::string &name) {
        if (timing || !cur_event.empty()) {
            fprintf(stderr, "\033[0;35mWARNING:\033[0m    There is an ongoing timing\n");
        }
        else if (events.find(name) != events.end()) {
            auto &e = events[name];
            e.starts.push_back({std::chrono::steady_clock::now()});
            timing = true;
            cur_event = name;
        }
        else {
            event e;
            e.name = name;
            e.starts.push_back({std::chrono::steady_clock::now()});
            events.emplace(std::piecewise_construct, std::forward_as_tuple(name), std::forward_as_tuple(e));
            timing = true;
            cur_event = name;

            events_seq.push_back(name);
        }
    }

    void stop() {
        if (!timing) {
            fprintf(stderr, "\033[0;35mWARNING:\033[0m    Timer was not started\n");
        }
        else {
            auto &e = events[cur_event];
            e.stops.push_back({std::chrono::steady_clock::now()});
            timing = false;
            cur_event = "";
        }
    }

    void print() {
        for (auto &n : events_seq) {
            auto &e = events.find(n)->second;
            assert(e.starts.size() == e.stops.size());
            uint64_t total = 0;
            for (std::size_t t = 0; t < e.starts.size(); t++) {
                auto d = std::chrono::duration_cast<std::chrono::microseconds>(e.stops[t] - e.starts[t]).count();
                total += d;
            }
            printf("\033[0;32mINFO:\033[0m    %s: %f ms\n", e.name.c_str(), (total / (e.starts.size() * 1000.0)));
        }
    }

    void print_to_csv(const std::string &f, const std::string &mark, bool append = true) {
        std::ofstream ofs;
        if (append) {
            ofs.open(f, std::ios_base::app);
        }
        else {
            ofs.open(f);
        }

        if (!ofs) {
            fprintf(stderr, "\033[0;35mWARNING:\033[0m    Cannot open CSV file\n");
        }
        else {
            auto iter = events.begin();
            for (std::size_t t = 0; t < iter->second.starts.size(); t++) {
                /* assuming all events were measured the same number of times */
                ofs << mark;
                for (auto &n : events_seq) {
                    auto &e = events.find(n)->second;
                    assert(e.starts.size() == e.stops.size());
                    auto d = std::chrono::duration_cast<std::chrono::microseconds>(e.stops[t] - e.starts[t]).count();
                    ofs << "," << (d / 1000.0);
                }
                ofs << "\n";
            }
            ofs.flush();

            // ofs << mark;
            // for (auto &[n, e] : events) {
            //     assert(e.starts.size() == e.stops.size());
            //     uint64_t total = 0;
            //     for (std::size_t t = 0; t < e.starts.size(); t++) {
            //         auto d = std::chrono::duration_cast<std::chrono::microseconds>(e.stops[t] - e.starts[t]).count();
            //         total += d;
            //     }
            //     ofs << "," << (total / (e.starts.size() * 1000.0));
            // }
            // ofs << "\n";
            // ofs.flush();
        }

        ofs.close();
    }

private:
    std::map<std::string, event> events;
    std::vector<std::string> events_seq;
    bool timing = false;
    std::string cur_event;
};

#else

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

#endif