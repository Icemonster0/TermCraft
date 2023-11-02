#include "engine.hpp"
#include "cl_option_manager.hpp"
#include "user_settings.hpp"

#include <cstdio>

user_settings U;

void process_command_line_options(int argc, char const *argv[]) {
    CL_Option_Manager clom;

    clom.set_user_hint("\
Usage:    TermCraft [<setting> <value>] [<flag>] ...\n\
Example:  TermCraft fps 12 --cursor-visible\n\
\n\
settings:\n\
 name              default value       description\n\
  fps               24                  Target fps\n\
  width             80                  Window width (if --fixed-window-size is set)\n\
  height            24                  Window height (if --fixed-window-size is set)\n\
  color-mode        FULL                FULL: rgb color\n\
                                        COMPAT: bw color (if FULL is not supported)\n\
  sky-color         0x7ce1ff            Hex code of sky color\n\
  render-distance   100                 Render distance in blocks\n\
  fog               0.5                 Fog factor (0.0 to 1.0)\n\
\n\
flags:\n\
 name                              description\n\
  --help                            Displays this help\n\
  --cursor-visible                  Makes the cursor not hidden (compat)\n\
  --fixed-window-size               Enables the width and height settings\n\
                                    (if not set: automatic window size)\n\
  --debug-info                      Show debug info on screen\n\
    ");

    clom.register_flag("--help");
    clom.register_flag("--cursor-visible");
    clom.register_setting("fps", "24");
    clom.register_flag("--fixed-window-size");
    clom.register_setting("width", "80");
    clom.register_setting("height", "24");
    clom.register_setting("color-mode", "FULL");
    clom.register_setting("sky-color", "0x7ce1ff");
    clom.register_setting("render-distance", "100");
    clom.register_setting("fog", "0.5");
    clom.register_flag("--debug-info");

    clom.process_cl_options(argc, argv);

    if (clom.is_flag_set("--help")) {
        clom.print_user_hint();
        exit(0);
    }

    try {
        U.cursor_visible = clom.is_flag_set("--cursor-visible");
        U.fps = std::stoi(clom.get_setting_value("fps"));
        U.fixed_window_size = clom.is_flag_set("--fixed-window-size");
        U.width = std::stoi(clom.get_setting_value("width"));
        U.height = std::stoi(clom.get_setting_value("height"));
        U.color_mode = clom.get_setting_value("color-mode");

        unsigned int col = std::stoi(clom.get_setting_value("sky-color"), nullptr, 16);
        unsigned int r = (col >> 16) & 0xff;
        unsigned int g = (col >> 8) & 0xff;
        unsigned int b = col & 0xff;
        U.sky_color = glm::vec3 {(float)r/255.0f, (float)g/255.0f, (float)b/255.0f};

        U.render_distance = std::stof(clom.get_setting_value("render-distance"));
        U.fog = std::stof(clom.get_setting_value("fog"));
        U.debug_info = clom.is_flag_set("--debug-info");
    }
    catch (...) {
        printf("Failed to parse command line options! Check if all settings expecting a number actually receive a number.\n");
        clom.print_user_hint();
        exit(1);
    }
}

int main(int argc, char const *argv[]) {

    process_command_line_options(argc, argv);

    tc::Engine engine {};
    int result = engine.run();
    printf("Engine exited with return value %d\n", result);

    return result;
}
