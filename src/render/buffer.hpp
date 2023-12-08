#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <vector>

namespace tc {

template <typename T>
struct buffer {
    buffer(int x_size, int y_size, T value) {
        clear(x_size, y_size, value);
    }

    buffer() {
    }

    void clear(int x_size, int y_size, T value) {
        buf.clear();
        buf = std::vector<std::vector<T>> (static_cast<long unsigned int>(x_size), std::vector<T> {});
        for (auto &column : buf) {
            column = std::vector<T> (static_cast<long unsigned int>(y_size), value);
        }
    }

    std::vector<std::vector<T>> buf;
};

} /* end of namespace tc */

#endif /* end of include guard: BUFFER_HPP */
