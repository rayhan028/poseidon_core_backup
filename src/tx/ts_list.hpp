#ifndef ts_list_hpp_
#define ts_list_hpp_

#include <list>
#include <thread>

template <typename T> struct ts_list {
    using const_iterator = typename std::list<T>::const_iterator;
    using iterator = typename std::list<T>::iterator;
 
    std::list<T> the_list_;
    std::mutex mtx_;
    std::unique_lock<std::mutex> lck_;

    ts_list() : lck_(mtx_, std::defer_lock) {}
    ts_list(const ts_list& ts) : the_list_(ts.the_list_) {}

    ~ts_list() = default;

    inline decltype(auto) empty() const { return the_list_.empty(); }

    inline decltype(auto) begin() { return the_list_.begin(); }
    inline decltype(auto) end() { return the_list_.end(); }

    inline decltype(auto) front() { return the_list_.front(); }

    template <class Predicate>
    void remove_if (Predicate pred) { the_list_.remove_if(pred); }

    inline void push_front(T&& val) { the_list_.push_front(std::move(val)); }
    inline void pop_front() { the_list_.pop_front(); }

    inline iterator erase(const_iterator first, const_iterator last) { return the_list_.erase(first, last); }

    inline void lock() { lck_.lock(); }
    inline void unlock() { lck_.unlock(); }

    inline std::lock_guard<std::mutex> lock_guard() { return std::lock_guard<std::mutex> (mtx_); }
};

#endif