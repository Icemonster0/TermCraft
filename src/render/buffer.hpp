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
        for (int x = 0; x < x_size; ++x) {
            buf.emplace_back();
            for (int y = 0; y < y_size; ++y) {
                buf[x].emplace_back(value);
            }
        }
    }

    std::vector<std::vector<T>> buf;
};

} /* end of namespace tc */

#endif /* end of include guard: BUFFER_HPP */
