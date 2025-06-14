#pragma once

#include <type_traits>
#include <iterator>

template <typename ForwardIt, typename T>
ForwardIt UpperBound(ForwardIt first, ForwardIt last, const T& value) {
    using difference_type = 
        typename std::iterator_traits<ForwardIt>::difference_type;

    difference_type count = std::distance(first, last);

    ForwardIt it;
    difference_type step;

    while (count > 0) {
        it = first;
        step = count / 2;
        std::advance(it, step);

        if (value < *it) {
            count = step;
        } else {
            first = ++it;
            count -= step + 1;
        }
    }

    return first;
}
