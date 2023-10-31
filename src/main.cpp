#include "engine.hpp"
#include "cl_option_manager.hpp"
#include "user_settings.hpp"

#include <cstdio>

user_settings U;

int main(int argc, char const *argv[]) {

    CL_Option_Manager clom;
    clom.add_flag("--cursor-visible");
    clom.process_cl_options(argc, argv);

    U.cursor_invis = !clom.is_flag_set("--cursor-visible");

    tc::Engine engine {80, 24, 24}; // X res, Y res, target fps
    int result = engine.run();
    printf("Engine exited with return value %d\n", result);

    return result;
}
