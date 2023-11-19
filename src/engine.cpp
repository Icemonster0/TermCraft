#include "engine.hpp"
#include "render/render.hpp"
#include "controller/controller.hpp"
#include "user_settings.hpp"

#include <cstdlib>
#include <chrono>
#include <thread>
#include <cstdio>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

namespace tc {

// public:

Engine::Engine() {
    system_catch_error("tput clear", 7);
    if (!U.fixed_window_size) system_catch_error("mkdir -p tmp.term_craft", 1);

    update_window_size();
    render = Render {X_size, Y_size};

    world = World {};
    world.generate(0, // seed
                  {U.world_size, U.world_size}); // world size (in chunks)

    glm::ivec2 center = world.get_world_center();
    controller = Controller {glm::vec3(center.x+0.5f, world.get_ground_height_at(center), center.y+0.5f), // spawn position
                             static_cast<float>(X_size) / static_cast<float>(Y_size), // aspect
                             1.62f, // height (eye level)
                             10.0f, // interact range
                             10.0f, // move speed (blocks per second)
                             40.0f, // look sensitivity (degrees per second)
                             &world}; // world pointer

    /* Player position needs to be known for optimized world meshing. */
    world.generate_initial_mesh();
}

int Engine::run() {
    if (!U.cursor_visible) system_catch_error("tput civis", 5);
    system_catch_error("stty -echo cbreak", 6);
    system_catch_error("tput clear", 7);

    input_thread = thread(&Engine::input_loop, this);
    render_thread = thread(&Engine::render_loop, this);

    input_thread.join(); // wait until user quits
    render_thread.join(); // ensures clean exit

    if (!U.cursor_visible) system_catch_error("tput cnorm", 8);
    system_catch_error("stty echo -cbreak", 9);
    system_catch_error("tput clear", 7);
    if (!U.fixed_window_size) system_catch_error("rm -r tmp.term_craft", 3);

    return status;
}

// private:

void Engine::input_loop() {
    char key = 0;

    while(!process_should_stop) {
        key = getchar();

        switch (key) {
            case 'q': process_should_stop = true; break;
            default: controller.input_event(key); break;
        }
    }
}

void Engine::render_loop() {
    float corrected_fps = U.fps;

    while (!process_should_stop) {
        chrono::high_resolution_clock timer;
        auto timer_start = timer.now();

        this_thread::sleep_for(chrono::microseconds(int(1000000.0f / corrected_fps)));

        update_window_size();
        controller.simulation_step(delta_time);
        render.set_params(X_size, Y_size, global_time, controller.get_V_matrix(), controller.get_VP_matrix(), controller.get_active_block_type());
        if (U.debug_info) render.set_debug_info(debug_info_string());

        system_catch_error("tput cup 0 0", 4);
        render.render(world.get_mesh());

        auto timer_end = timer.now();
        delta_time = chrono::duration_cast<chrono::milliseconds>(timer_end - timer_start).count() / 1000.0f;
        global_time += delta_time;
        fps = 1.0f / delta_time;
        corrected_fps += static_cast<float>(U.fps) - fps;
    }
}

string Engine::debug_info_string() {
    int n_tris;
    int n_active_tris;
    render.get_params(&n_tris, &n_active_tris);

    glm::vec3 pos;
    glm::vec2 look;
    controller.get_params(&pos, &look);

    float est_memory = (float)world.estimate_memory_usage() / 1000000.0f;

    stringstream ss;

    ss << "Debug info\n";

    ss << "fps: " << static_cast<int>(fps) << "\n";
    ss << "screen: " << X_size << "x" << Y_size << "\n";
    ss << "time: " << global_time << "\n";
    ss << "coords: " << pos.x << " " << pos.y << " " << pos.z << "\n";
    ss << "yaw: " << look.x << " deg\n";
    ss << "pitch: " << look.y << " deg\n";
    ss << "tris: " << n_tris << "\n";
    ss << "active tris: " << n_active_tris << "\n";
    ss << "est. memory: " << est_memory << "MB\n";

    return ss.str();
}

void Engine::update_window_size() {
    if (U.fixed_window_size) {
        X_size = U.width;
        Y_size = U.height;
    }
    else {
        system_catch_error("tput cols >> tmp.term_craft/term-size.tmp", 2);
        system_catch_error("tput lines >> tmp.term_craft/term-size.tmp", 2);

        ifstream file("tmp.term_craft/term-size.tmp");
        if (!file.is_open()) {
            crash(11);
            return;
        }

        old_X_size = X_size;
        old_Y_size = Y_size;

        file >> X_size;
        file >> Y_size;

        file.close();

        system_catch_error("> tmp.term_craft/term-size.tmp", 10);

        if (X_size != old_X_size && Y_size != old_Y_size) {
            controller.update_aspect(static_cast<float>(X_size) / static_cast<float>(Y_size));
        }
    }
}

void Engine::system_catch_error(string command, int code) {
    if (system(command.c_str())) {
        crash(code);
    }
}

void Engine::crash(int code) {
    status = code;
    process_should_stop = true;
}

} /* end of namespace tc */
