#include "engine.hpp"
#include "render/render.hpp"
#include "controller/controller.hpp"

#include <cstdlib>
#include <chrono>
#include <thread>
#include <cstdio>
#include <fstream>
#include <string>

using namespace std;

namespace tc {

Engine::Engine(int p_X_size, int p_Y_size, int p_target_fps) : X_size(p_X_size), Y_size(p_Y_size), target_fps(p_target_fps) {
    render = Render {X_size, Y_size};
    world = World {0};
    controller = Controller {glm::vec3(0.0f, world.get_spawn_height(), 0.0f),
                             static_cast<float>(X_size) / static_cast<float>(Y_size),
                             1.0f, 2.0f, 60.0f};
}

int Engine::run() {
    system_catch_error("tput civis", 5);
    system_catch_error("stty -echo cbreak", 6);
    system_catch_error("tput clear", 7);

    input_thread = thread(&Engine::input_loop, this);
    render_thread = thread(&Engine::render_loop, this);

    input_thread.join(); // wait until user quits
    render_thread.join(); // ensures clean exit

    system_catch_error("tput cnorm", 8);
    system_catch_error("stty echo -cbreak", 9);
    system_catch_error("tput clear", 7);

    return status;
}

void Engine::input_loop() {
    char key = 0;

    while(!process_should_stop) {
        key = getchar();

        switch (key) {
            case 'q': process_should_stop = true; break;
            default: controller.input_event(key);
        }
    }
}

void Engine::render_loop() {
    while (!process_should_stop) {
        chrono::high_resolution_clock timer;
        auto timer_start = timer.now();

        this_thread::sleep_for(chrono::microseconds(int(1000000.0f / target_fps)));

        update_window_size();
        controller.simulation_step(delta_time);
        render.set_params(X_size, Y_size, global_time, controller.get_VP_matrix());

        system_catch_error("tput cup 0 0", 4);
        render.render(world.get_mesh());

        auto timer_end = timer.now();
        delta_time = chrono::duration_cast<chrono::milliseconds>(timer_end - timer_start).count() / 1000.0f;
        global_time += delta_time;

        debug_info();
    }
}

void Engine::debug_info() {
    system_catch_error("tput cup 0 0", 4);
    printf("Debug info\n");

    int n_tris;
    int n_active_tris;
    render.get_params(&n_tris, &n_active_tris);

    glm::vec3 pos;
    glm::vec2 look;
    controller.get_params(&pos, &look);

    printf("fps: %d\n", (int)(1.0f / delta_time));
    printf("screen: %dx%d\n", X_size, Y_size);
    printf("time: %.2f\n", global_time);
    printf("coords: %.2f %.2f %.2f\n", pos.x, pos.y, pos.z);
    printf("yaw: %.2f°\n", look.x);
    printf("pitch: %.2f°\n", look.y);
    printf("tris: %d\n", n_tris);
    printf("active tris: %d\n", n_active_tris);
}

void Engine::update_window_size() {
    system_catch_error("mkdir -p tmp", 1);
    system_catch_error("tput cols >> tmp/term-size.tmp", 2);
    system_catch_error("tput lines >> tmp/term-size.tmp", 2);

    ifstream file("tmp/term-size.tmp");
    if (!file.is_open()) {
        crash(5);
        return;
    }

    file >> X_size;
    file >> Y_size;

    file.close();

    system_catch_error("rm tmp/term-size.tmp", 3);

    controller.update_aspect(static_cast<float>(X_size) / static_cast<float>(Y_size));
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
