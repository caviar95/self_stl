#pragma once

#include <string>

class Event {
public:
    virtual ~Event() = default;
    virtual const std::string& type() const = 0;
};

class NewsEvent : public Event {
public:
    NewsEvent(const std::string& news) : news_(news) {}

    const std::string& type() const override {
        static const std::string type = "news";
        return type;
    }

    const std::string &content() const {
        return news_;
    }

private:
    std::string news_;
};

class AdEvent : public Event {
public:
    AdEvent(const std::string& ad) : ad_(ad) {}

    const std::string& type() const override {
        static const std::string type = "ad";
        return type;
    }

    const std::string &content() const {
        return ad_;
    }

private:
    std::string ad_;
};
