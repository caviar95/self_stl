#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <optional>

template <size_t N>
struct CombinedState {
    std::array<uint64_t, N> subs{};

    CombinedState() {subs.fill(0);}

    static CombinedState FromArray(const std::array<uint64_t, N>& a) {
        CombinedState s;
        s.subs = a;
        return s;
    }

    CombinedState operator|(const CombinedState& o) const {
        CombinedState r;
        for (size_t i = 0; i < N; ++i) {
            r.subs[i] = subs[i] | o.subs[i];
        }

        return r;
    }

    CombinedState operator&(const CombinedState& o) const {
        CombinedState r;
        for (size_t i = 0; i < N; ++i) {
            r.subs[i] = subs[i] & o.subs[i];
        }

        return r;
    }

    bool operator==(const CombinedState& o) const {
        return subs == o.subs;
    }

    bool operator!=(const CombinedState&o) const {
        return !(*this == o);
    }

    bool ContainsAll(const CombinedState& other) const {
        for (size_t i = 0; i < N; ++i) {
            if ((subs[i] & other.subs[i]) != other.subs[i]) {
                return false;
            }
        }

        return true;
    }

    bool OverlapsAllSubsystems(const CombinedState& other) const {
        for (size_t i = 0; i < N; ++i) {
            if ((subs[i] & other.subs[i]) == 0) {
                return false;
            }
        }

        return true;
    }

    bool OverlapsAny(const CombinedState& other) const {
        for (size_t i = 0; i < N; ++i) {
            if ((subs[i] & other.subs[i]) != 0) {
                return true;
            }
        }

        return false;
    }

    void ValidateWidths(const std::array<int, N>& widths) const {
        for (size_t i = 0; i < N; ++i) {
            if (widths[i] <= 0 || widths[i] > 64) {
                throw std::invalid_argument("invalid argument");
            }

            uint64_t mask = (widths[i] == 64) ? ~uint64_t(0) : ((uint64_t(1) << widths[i]) - 1);

            if ((subs[i] & ~mask) != 0) {
                std::ostringstream oss;
                oss << "CombinedState::subsystem " << i << " has bits outside width " << widths[i];
                throw std::out_of_range(oss.str());
            }
        }
    }

    std::string ToString() const {
        std::ostringstream oss;
        oss << "{";
        for (size_t i = 0; i < N; ++i) {
            if (i) {
                oss << ", ";
                oss << "sub" << i << ":0x" << std::hex << subs[i] << std::dec;
            }
        }
        oss << "}";

        return oss.str();
    }
};

template <size_t N>
struct FinalState {
    std::string name;
    CombinedState<N> mask;
    int priority = 0;

    FinalState() = default;
    FinalState(std::string n, CombinedState<N> m, int p = 0) : name(std::move(n)), mask(m), priority(p) {}
};

template <size_t N>
class MaskStateMachine {
public:
    using CS = CombinedState<N>;
    using FS = FinalState<N>;

    MaskStateMachine(const std::array<int, N>& widths) : widths_(widths) {
        for (int w : widths_) {
            if (w <= 0 || w > 64) {
                throw std::invalid_argument("widths must be in 1..64");
            }
        }
    }

    void RegisterFinal(const FS& f) {
        f.mask.ValidateWidths(widths_);
        finals_.push_back(f);
    }

    std::vector<FS> MatchFinalStates(const CS& current, const std::string& matchMode = "contains") const {
        std::vector<FS> out;
        for (const auto& f : finals_) {
            bool ok = false;
            if (matchMode == "contains") {
                ok = current.ContainsAll(f.mask);
            } else if (matchMode == "OverlapsAllSubsystems") {
                ok = current.OverlapsAllSubsystems(f.mask);
            } else if (matchMode == "OverlapsAny") {
                ok = current.OverlapsAny(f.mask);
            } else {
                throw std::invalid_argument("unknown match mode");
            }

            if (ok) {
                out.push_back(f);
            }
        }

        std::sort(out.begin(), out.end(), [](const FS& a, const FS& b) {
            if (a.priority != b.priority) {
                return a.priority > b.priority;
            }  

            return lexicographic_mask_cmp<N>(a.mask, b.mask) > 0;
        });

        return out;
    }

    std::optional<CS> HighestOfSet(const std::vector<CS>& set, const std::optional<std::array<uint64_t, N>>& priorityWeight = std::nullopt) const {
        if (set.empty()) {
            return std::nullopt;
        }

        auto cmp = [&](const CS& a, const CS& b) -> bool {
            if (priorityWeight) {
                __int128 va = 0;
                __int128 vb = 0;

                for (size_t i = 0; i < N; ++i) {
                    va = va * (__int128(priorityWeight->at(i) + 1)) + __int128(a.subs[i]);
                    vb = vb * (__int128(priorityWeight->at(i) + 1)) + __int128(b.subs[i]);
                }

                return va > vb;
            } else {
                return lexicographic_mask_cmp<N>(a, b) > 0;
            }
        };

        const CS* best = &set[0];

        for (size_t i = 0; i < N; ++i) {
            if (cmp(set[i], *best)) {
                best = &set[i];
            }
        }

        return *best;
    }

    const std::vector<FS>& Finals() const {
        return finals_;
    }

private:
    std::array<int, N> widths_;
    std::vector<FS> finals_;

    template <size_t M>
    static int lexicographic_mask_cmp(const CombinedState<M>& a, const CombinedState<M>& b) {
        for (size_t i = 0; i < M; ++i) {
            if (a.subs[i] > b.subs[i]) {
                return 1;
            }

            if (a.subs[i] < b.subs[i]) {
                return -1;
            }
        }

        return 0;
    }
};
