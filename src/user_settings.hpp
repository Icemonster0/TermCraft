#ifndef USER_SETTINGS_HPP
#define USER_SETTINGS_HPP

#include "glm.hpp"

#include <string>

struct user_settings {
    bool cursor_visible;

    int world_size;
    int seed;
    bool no_caves;

    int fps;
    float fov;
    float look_sensitivity;
    bool noclip;

    bool fixed_window_size;
    int width;
    int height;

    std::string color_mode;
    bool disable_textures;

    glm::vec3 sky_color;
    float render_distance;
    float fog;

    bool debug_info;
    bool bad_normals;
    bool hide_hud;

    float start_time;
    float time_scale;
};

/* This object needs to be declared in main.cpp */
extern user_settings U;

#endif /* end of include guard: USER_SETTINGS_HPP */
