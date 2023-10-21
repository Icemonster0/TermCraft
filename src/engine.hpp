#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "render/render.hpp"
#include "controller/controller.hpp"
#include "world/world.hpp"

#include <thread>
#include <string>

namespace tc {

class Engine {
public:
    Engine(int p_X_size, int p_Y_size, int p_target_fps);

    int run();

private:
    void input_loop();
    void render_loop();

    void debug_info();
    void update_window_size();
    void system_catch_error(std::string command, int code);
    void crash(int code);

    Render render;
    World world;
    Controller controller;

    int X_size;
    int Y_size;
    int target_fps;
    float delta_time = 0.0f;
    float global_time = 0.0f;

    std::thread input_thread;
    std::thread render_thread;

    bool process_should_stop = false;
    int status = 0;
};

} /* end of namespace tc */

#endif /* end of include guard: ENGINE_HPP */
