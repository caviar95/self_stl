#pragma once

#include <bitset>
#include <vector>
#include <string>
#include <unordered_map>
#include <shared_mutex>
#include <functional>
// #include <nlohmann/json.hpp>

template <size_t N>
struct CombinedMask {
    std::array<std::bitset<64>, N> bits;
};

template <size_t N>
struct FinalState {
    std::string name;
    CombinedMask<N> required;
    CombinedMask<N> forbidden;
    CombinedMask<N> except;

    bool matches(const CombinedMask<N>& current) const {
        for (size_t i = 0; i < N; ++i) {
            if ((current.bits[i] & forbidden.bits[i]).any()) {
                return false;
            }

            if ((current.bits[i] & required.bits[i]) != required.bits[i]) {
                return false;
            }
        }

        return true;
    }
};

template <size_t N>
class MaskStateMachine {
public:
    using Callback = std::function<void(const std::string&)>;

    void addFinalState(FinalState<N> fs) {
        std::unique_lock lock(mutex_);
        finals_.push_back(std::move(fs));
        // build index-reversed
        size_t idx = finals_.size() - 1;
        for (size_t i = 0; i < N; ++i) {
            auto bits = finals_[idx].required.bits[i];

            for (size_t b = 0; b < bits.size(); ++b) {
                if (bits.test(b)) {
                    index_[i][b].push_back(idx);
                }
            }
        }
    }

    void setSubsystemState(size_t subsystem, size_t bitpos, bool value = true) {
        {
            std::unique_lock lock(mutex_);
            current_.bits[subsystem].reset();
            current_.bits[subsystem].set(bitpos, value);
        }

        evaluate();
    }

    void onFinalChanged(Callback cb) {
        callback_ = std::move(cb);
    }

protected:
    void evaluate() {
        std::shared_lock lock(mutex_);
        for (size_t i = 0; i < finals_.size(); ++i) {
            if (finals_[i].matches(current_)) {
                if (lastFinal_ != finals_[i].name) {
                    lastFinal_ = finals_[i].name;
                    if (callback_) {
                        callback_(lastFinal_);
                    }
                }

                return;
            }
        }

        if (lastFinal_ != "") {
            lastFinal_ = "";
            if (callback_) {
                callback_("");
            }
        }
    }

    CombinedMask<N> current_;
    std::vector<FinalState<N>> finals_;
    std::string lastFinal_;
    Callback callback_;
    std::unordered_map<size_t, std::unordered_map<size_t, std::vector<size_t>>> index_;
    mutable std::shared_mutex mutex_;
};