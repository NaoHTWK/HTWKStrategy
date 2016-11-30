#ifndef ALGORITHM_EXT_H
#define ALGORITHM_EXT_H

#include <algorithm>

namespace htwk {

template <class Container,class Compare>
auto min_element(Container& c, Compare comp) {
    return std::min_element(c.begin(), c.end(), comp);
}

template <class Container>
auto min_element(Container& c) {
    return std::min_element(c.begin(), c.end());
}

template <class Container,class Compare>
auto max_element(Container& c, Compare comp) {
    return std::max_element(c.begin(), c.end(), comp);
}

template <class Container>
auto max_element(Container& c) {
    return std::max_element(c.begin(), c.end());
}

template <class Container, class OutputIterator, class UnaryPredicate>
OutputIterator copy_if (const Container& in, OutputIterator out, UnaryPredicate pred) {
    return std::copy_if(in.begin(), in.end(), out, pred);
}

template <class T, class Container>
auto count(Container& in, const T& val) {
    return std::count(in.begin(), in.end(), val);
}

}  // namespace htwk

#endif  // ALGORITHM_EXT_H
