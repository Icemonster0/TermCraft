#include "engine.hpp"
#include "render/render.hpp"

#include <cstdlib>
#include <chrono>
#include <thread>
#include <cstdio>

using namespace std;

namespace tc {

Engine::Engine(int X_size, int Y_size, int p_target_fps) : target_fps(p_target_fps) {
    render = Render {X_size, Y_size};
}

int Engine::run() {
    system("tput civis");
    system("stty -echo cbreak");
    system("tput clear");

    input_thread = thread(&Engine::input_loop, this);
    render_thread = thread(&Engine::render_loop, this);

    input_thread.join(); // wait until user quits
    render_thread.join(); // ensures clean exit

    system("tput cnorm");
    system("stty echo -cbreak");
    system("tput clear");

    return 0;
}

void Engine::input_loop() {
    char key = 0;

    while(key != 27) { // 27 = escape
        key = getchar();

        switch (key) {
            default: break;
        }
    }

    process_should_stop = true;
}

void Engine::render_loop() {
    while (!process_should_stop) {
        chrono::high_resolution_clock timer;
        auto timer_start = timer.now();

        this_thread::sleep_for(chrono::microseconds(int(1000000.0f / target_fps)));

        render.render(global_time);

        auto timer_end = timer.now();
        delta_time = chrono::duration_cast<chrono::milliseconds>(timer_end - timer_start).count() / 1000.0f;
        global_time += delta_time;

        debug_info(delta_time);
    }
}

void Engine::debug_info(float delta_time) {
    system("tput cup 0 0");
    printf("Debug info\n");

    printf("fps: %d\n", (int)(1.0f / delta_time));
}

} /* end of namespace tc */
