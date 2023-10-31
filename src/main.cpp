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
 name          default value       description\n\
  fps           24                  Target fps\n\
  width         80                  Window width (if --fixed-window-size is set)\n\
  height        24                  Window height (if --fixed-window-size is set)\n\
  color-type    FULL                FULL: rgb color\n\
                                    COMPAT: bw color (if FULL is not supported)\n\
\n\
flags:\n\
 name                              description\n\
  --help                            Displays this help\n\
  --cursor-visible                  Makes the cursor not hidden (compat)\n\
  --fixed-window-size               Enables the width and height settings\n\
                                    (if not set: automatic window size)\n\
    ");

    clom.register_flag("--help");
    clom.register_flag("--cursor-visible");
    clom.register_setting("fps", "24");
    clom.register_flag("--fixed-window-size");
    clom.register_setting("width", "80");
    clom.register_setting("height", "24");
    clom.register_setting("color-type", "FULL");

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
        U.color_type = clom.get_setting_value("color-type");
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
