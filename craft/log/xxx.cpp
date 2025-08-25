// 平台

template <class... Args>
int Report(bool mode, boost::location::source_location loc = boost::location::source_location::current(), Args&&... args) {
    if (mode) {
        return 0;
    }
    std::cerr << "Error at " << loc.file_name() << ":" << loc.line() << ": ";
    (std::cerr << ... << args) << std::endl;
    return -1;
}


// 开发模块

class ILoggingAdapter {
public:
    virtual ~ILoggingAdapter() = default;

    template <typename... Args>
    void ReportInfo(bool mode, Args&&... args) {
        DoReport(mode, ToStringVector(std::forward<Args>(args)...));
    }

protected:
    virtual void DoReport(bool mode, const std::vector<std::string>& args) = 0;

    template <typename T>
    std::string ToString(const T& val) const {
        std::ostringstream oss;
        oss << val;
        return oss.str();
    }

    std::string ToString(const std::string& s) const { return s; }
    std::string ToString(const char* s) const { return std::string(s); }

    template <typename... Args>
    std::vector<std::string> ToStringVector(Args&&... args) const {
        return { ToString(std::forward<Args>(args))... };
    }
};


class DefaultLoggingAdapter : public ILoggingAdapter {
public:
    void DoReport(int level, int code, const std::vector<std::string>& args) override {
        ReportFromVector(level, code, args, 0);
    }

private:
    template <typename... Args>
    void Report(bool mode, Args&&... args) {
        Print(mode, boost::location::source_location::current(), std::forward<Args>(args)...);
    }

    template <typename... Args>
    void ReportFromVector(bool mode, const std::vector<std::string>& vec, size_t index, Args&&... args) {
        if (index == vec.size()) {
            Report(level, code, std::forward<Args>(args)...);
        } else {
            ReportFromVector(level, code, vec, index + 1, std::forward<Args>(args)..., vec[index]);
        }
    }
};

#define REPORTERROR(mode, ...) \
    logger->ReportInfo(mode, boost::location::source_location::current(), ##__VA_ARGS__)