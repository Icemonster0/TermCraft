#ifndef USER_SETTINGS_HPP
#define USER_SETTINGS_HPP

#include "glm.hpp"

#include <string>

struct user_settings {
    bool cursor_visible = false;

    int world_size = 10;

    int fps = 24;
    float fov = 70.0f;
    float look_sensitivity = 90.0f;
    bool noclip = false;

    bool fixed_window_size = false;
    int width = 80;
    int height = 24;

    std::string color_mode = "FULL";
    bool disable_textures = false;

    glm::vec3 sky_color = {0.486f, 0.882f, 1.0f};
    float render_distance = 100.0f;
    float fog = 0.5f;

    bool debug_info = false;
    bool bad_normals = false;
    bool hide_hud = false;

    float start_time = 10.0f;
    float time_scale = 60.0f;
};

/* This object needs to be declared in main.cpp */
extern user_settings U;

#endif /* end of include guard: USER_SETTINGS_HPP */
