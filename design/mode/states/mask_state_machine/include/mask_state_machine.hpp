#pragma once

#include <type_traits>

template <typename E>
constexpr auto to_underlying(E e) noexcept -> std::underlying_type_t<E> {
    return static_cast<std::underlying_type_t<E>>(e);
}

#define ENUM_DECLARE_ELEM(x) x,
#define ENUM_STRING_ELEM(x) #x

#define DECLARE_ENUM_WITH_STRINGS(NAME, LIST) \
    enum class NAME {LIST(ENUM_DECLARE_ELEM)}; \
    inline const char* to_string(NAME v) { \
        static const char* _names[] = { LIST(ENUM_STRING_ELEM) }; \
        auto idx = static_cast<size_t>(to_underlying(v)); \
        return (idx < sizeof(_names[0])) ? _names[idx] : "Unknown"; \
    }

template <typename E>
struct SubMatcher {
    enum class Kind {Any, Equals, InSet, NotEquals, NotInSet, Predicate};

    Kind kind = Kind::Any;

    std::vector<E> values;
    std::unordered_set<std::underlying_type_t<E>> setVals;
    std::function<bool(E)> pred;

    SubMatcher() = default;

    static SubMatcher Any() { return SubMatcher(); }
    static SubMatcher Equals(E v) {
        SubMatcher m;
        m.kind = Kind::Equals;
        m.values = {v};
    }
};