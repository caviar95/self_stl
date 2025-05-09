#pragma once

template <typename T>
class basic_stopwatch : public T {
    typedef typename T BaseTimer;

public:
    explicit basic_stopwatch(bool start);
    explicit basic_stopwatch(char const* activity = "Stopwatch", bool start = true);
    basic_stopwatch(std::ostream& log, char const* activity = "Stopwatch", bool start = true);

    ~basic_stopwatch();

    unsigned LapGet() const;

    bool IsStarted() const;

    unsigned Show(char const *event = "show");

    unsigned Start(char const *event = "start");

    unsigned Stop(char const *event = "stop");

private:
    char const *activity_;
    unsigned lap_;
    std::ostream &log_;
};
