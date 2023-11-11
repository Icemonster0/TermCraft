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
    clom.register_setting<std::string>("color-mode", "FULL", "FULL: rgb color;\nCOMPAT: bw color (if FULL is not supported)");
    clom.register_setting<std::string>("sky-color", "0x7ce1ff", "Hex code of sky color (it says std::string, but is actually hexadecimal int, eg. 0x7ce1ff)");
    clom.register_setting<float>("render-distance", 100, "Render distance in blocks");
    clom.register_setting<float>("fog", 0.5f, "Fog factor (0.0 to 1.0)");
    clom.register_flag("--debug-info", "Show debug info in the HUD");
    clom.register_flag("--bad-normals", "Show face front in blue, back in red; Disable backface culling");
    clom.register_setting<float>("fov", 70.0f, "Field of view in degrees");
    clom.register_flag("--disable-textures", "Use flat colors instead of textures");
    clom.register_setting<int>("world-size", 10, "World x and z width in chunks");

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
}

int main(int argc, char const *argv[]) {

    process_command_line_options(argc, argv);

    tc::Engine engine {};
    int result = engine.run();
    printf("Engine exited with return value %d\n", result);

    return result;
}
