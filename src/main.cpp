#include "engine.hpp"
#include "clom.hpp"
#include "user_settings.hpp"

#include <cstdio>

user_settings U;

void process_command_line_options(int argc, char const *argv[]) {
    CL_Option_Manager clom;

    clom.register_flag("--help", "Display this help");
    clom.register_flag("--cursor-visible", "Make the cursor not hidden (compat)");
    clom.register_setting<int>("fps", 24, "Target fps");
    clom.register_flag("--fixed-window-size", "Enable the width and height settings (if not set: automatic window size)");
    clom.register_setting<int>("width", 80, "Window width (if --fixed-window-size is set)");
    clom.register_setting<int>("height", 24, "Window height (if --fixed-window-size is set)");
    clom.register_setting<std::string>("color-mode", "FULL", "FULL: rgb color\nCOMPAT: bw color (if FULL is not supported)\nASCII: bw as ascii art (for fun)");
    clom.register_setting<std::string>("sky-color", "0x7ce1ff", "Hex code of sky color (it says std::string, but is actually hexadecimal int, eg. 0x7ce1ff)");
    clom.register_setting<float>("render-distance", 100, "Render distance in blocks");
    clom.register_setting<float>("fog", 0.5f, "Fog factor (0.0 to 1.0)");
    clom.register_flag("--debug-info", "Show debug info in the HUD");
    clom.register_flag("--bad-normals", "Show face front in blue, back in red; Disable backface culling");
    clom.register_setting<float>("fov", 70.0f, "Field of view in degrees");
    clom.register_flag("--disable-textures", "Use flat colors instead of textures");
    clom.register_setting<int>("world-size", 10, "World x and z width in chunks");
    clom.register_setting<float>("start-time", 10.0f, "Starting time of day (in 24-hour clock)");
    clom.register_setting<float>("time-scale", 60.0f, "Speed up factor of time of day (1 = real life scale; 60 (default) = 24 in game hours hours last 24 real life minutes)");
    clom.register_setting<float>("look-sensitivity", 90.0f, "Look/turn speed in degrees per second");
    clom.register_flag("--noclip", "When flying, disable collisions");
    clom.register_flag("--hide-hud", "Disable the HUD");
    clom.register_setting<int>("seed", 0, "World generation seed");
    clom.register_flag("--no-caves", "Disable cave generation");

    clom.generate_user_hint("TermCraft");
    clom.process_cl_options(argc, argv);

    if (clom.is_flag_set("--help")) {
        clom.print_user_hint();
        exit(0);
    }

    U.cursor_visible = clom.is_flag_set("--cursor-visible");
    U.fps = clom.get_setting_value<int>("fps");
    U.fixed_window_size = clom.is_flag_set("--fixed-window-size");
    U.width = clom.get_setting_value<int>("width");
    U.height = clom.get_setting_value<int>("height");
    U.color_mode = clom.get_setting_value<std::string>("color-mode");

    unsigned int col = std::stoi(clom.get_setting_value<std::string>("sky-color"), nullptr, 16);
    unsigned int r = (col >> 16) & 0xff;
    unsigned int g = (col >> 8) & 0xff;
    unsigned int b = col & 0xff;
    U.sky_color = glm::vec3 {(float)r/255.0f, (float)g/255.0f, (float)b/255.0f};

    U.render_distance = clom.get_setting_value<float>("render-distance");
    U.fog = clom.get_setting_value<float>("fog");
    U.debug_info = clom.is_flag_set("--debug-info");
    U.bad_normals = clom.is_flag_set("--bad-normals");
    U.fov = clom.get_setting_value<float>("fov");
    U.disable_textures = clom.is_flag_set("--disable-textures");
    U.world_size = clom.get_setting_value<int>("world-size");
    U.start_time = clom.get_setting_value<float>("start-time");
    U.time_scale = clom.get_setting_value<float>("time-scale");
    U.look_sensitivity = clom.get_setting_value<float>("look-sensitivity");
    U.noclip = clom.is_flag_set("--noclip");
    U.hide_hud = clom.is_flag_set("--hide-hud");
    U.seed = clom.get_setting_value<int>("seed");
    U.no_caves = clom.is_flag_set("--no-caves");
}

void print_error_message(int result) {
    switch (result) {
        case  2: printf("Error: Failed to get terminal size and/or open pipe with `popen(\"tput cols; tput lines\", \"r\"))`. You might need to use the flag `--fixed-window-size`.\n"); break;
        case  4: printf("Error: Failed to set cursor to position zero (command: `tput cup 0 0`).\n"); break;
        case  5: printf("Error: Failed to make cursor invisible (command: `tput civis`). You might need to use the flag `--cursor-visible`.\n"); break;
        case  6: printf("Error: Input setup failed (command: `stty -echo cbreak`).\n"); break;
        case  7: printf("Error: Failed to clear the terminal window (command: `tput clear`).\n"); break;
        case  8: printf("Error: Failed to reset cursor to normal mode (command: `tput cnorm`). You might need to use the flag `--cursor-visible`.\n"); break;
        case  9: printf("Error: Failed to reset input to normal mode (command: `stty echo -cbreak`).\n"); break;
        case 10: printf("Error: Failed to read terminal size from pipe with `fscanf(...)`. You might need to use the flag `--fixed-window-size`.\n"); break;
    }
}

int main(int argc, char const *argv[]) {

    process_command_line_options(argc, argv);

    tc::Engine engine {};
    int result = engine.run();
    printf("Engine exited with code %d\n", result);
    print_error_message(result);

    return result;
}
