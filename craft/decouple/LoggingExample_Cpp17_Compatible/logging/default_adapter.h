#pragma once

#include "logging_adapter.h"
#include "../platform/platform.h"
#include <tuple>
#include <utility>

class DefaultLoggingAdapter : public ILoggingAdapter {
public:
    void DoLog(int level, int code, const std::vector<std::string>& args) override {
        auto tupleArgs = VectorToTuple(args);
        std::apply(
            [=](const auto&... unpackedArgs) {
                Print(level, code, unpackedArgs...);
            },
            tupleArgs
        );
    }

private:
    template <std::size_t... Is>
    auto VectorToTupleImpl(const std::vector<std::string>& vec, std::index_sequence<Is...>) {
        return std::make_tuple(vec[Is]...);
    }

    auto VectorToTuple(const std::vector<std::string>& vec) {
        return VectorToTupleImpl(vec, std::make_index_sequence<vec.size()>{});
    }
};