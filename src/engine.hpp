#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "render/render.hpp"

#include <thread>

namespace tc {

class Engine {
public:
    Engine(int X_size, int Y_size, int p_target_fps);

    int run();

private:
    void input_loop();
    void render_loop();

    void debug_info(float delta_time);

    Render render;
    // World world; TODO

    int target_fps;
    float delta_time = 0.0f;

    std::thread input_thread;
    std::thread render_thread;

    bool process_should_stop = false;
};

} /* end of namespace tc */

#endif /* end of include guard: ENGINE_HPP */
