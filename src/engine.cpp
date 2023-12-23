#include "engine.hpp"


using namespace std;

namespace tc {

// public:

Engine::Engine() {
    system_catch_error("tput clear", 7);

    update_window_size();
    render = Render {X_size, Y_size};

    world = World {};
    world.generate(U.seed, {U.world_size, U.world_size});

    glm::ivec2 center = world.get_world_center();
    controller = Controller {glm::vec3(center.x+0.5f, world.get_ground_height_at(center) - 0.5f, center.y+0.5f), // spawn position
                             static_cast<float>(X_size) / static_cast<float>(Y_size), // aspect
                             10.0f, // interact range
                             &world}; // world pointer

    /* Player position needs to be known for optimized world meshing. */
    world.generate_initial_mesh();
}

int Engine::run() {
    if (!U.cursor_visible) system_catch_error("tput civis", 5);
    system_catch_error("stty -echo cbreak", 6);
    // system_catch_error("tput clear", 7);

    input_thread = thread(&Engine::input_loop, this);
    render_thread = thread(&Engine::render_loop, this);

    input_thread.join(); // wait until user quits
    render_thread.join(); // ensures clean exit

    if (!U.cursor_visible) system_catch_error("tput cnorm", 8);
    system_catch_error("stty echo -cbreak", 9);
    system_catch_error("tput clear", 7);

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
        render.set_params(X_size, Y_size, global_time, time_of_day, controller.get_V_matrix(), controller.get_VP_matrix(), controller.get_active_block_type(), controller.is_flying(), controller.is_crouching(), controller.is_sprinting());
        if (U.debug_info) render.set_debug_info(debug_info_string());

        system_catch_error("tput cup 0 0", 4);
        render.render(world.get_mesh());

        auto timer_end = timer.now();
        delta_time = chrono::duration_cast<chrono::milliseconds>(timer_end - timer_start).count() / 1000.0f;
        global_time += delta_time;
        calc_time_of_day();
        fps = 1.0f / delta_time;
        corrected_fps += static_cast<float>(U.fps) - fps;
    }
}

string Engine::debug_info_string() {
    int n_tris;
    int n_active_tris;
    render.get_params(&n_tris, &n_active_tris);

    int time_of_day_hours = (int)floor(time_of_day * 24);

    glm::vec3 pos;
    glm::vec3 velocity;
    glm::vec2 look;
    controller.get_params(&pos, &velocity, &look);

    float est_memory = (float)world.estimate_memory_usage() / 1000000.0f;

    stringstream ss;

    ss << "Debug info\n";

    ss << std::fixed << std::setprecision(2);
    ss << "fps: " << static_cast<int>(fps) << "\n";
    ss << "screen: " << X_size << "x" << Y_size << "\n";
    ss << "running time: " << global_time << "s\n";
    ss << "in-game time: "  << setfill('0') << setw(2) << time_of_day_hours << ":"
       << setw(2) << (int)floor((time_of_day*24.0f - time_of_day_hours) * 60) << "\n";
    ss << "coords: " << pos.x << " " << pos.y << " " << pos.z << "\n";
    ss << "velocity: " << velocity.x << " " << velocity.y << " " << velocity.z << "\n";
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
        old_X_size = X_size;
        old_Y_size = Y_size;

        FILE *file;
        if (!(file = popen("tput cols; tput lines", "r"))) {
            crash(2);
        }

        if(fscanf(file, "%d %d", &X_size, &Y_size) != 2) { // ie. failed to read 2 numbers
            crash(10);
        }

        pclose(file);

        if (X_size != old_X_size || Y_size != old_Y_size) {
            controller.update_aspect(static_cast<float>(X_size) / static_cast<float>(Y_size));
        }
    }
}

void Engine::calc_time_of_day() {
    const float seconds_per_day = 86400.0f;
    time_of_day = global_time / seconds_per_day * U.time_scale + U.start_time / 24.0f;
    time_of_day -= floor(time_of_day);
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
