#ifndef USER_SETTINGS_HPP
#define USER_SETTINGS_HPP

#include <string>

struct user_settings {
    bool cursor_visible = false;
    int fps = 24;
    bool fixed_window_size = false;
    int width = 80;
    int height = 24;
    std::string color_type = "FULL";
};

/* This object needs to be declared in main.cpp */
extern user_settings U;

#endif /* end of include guard: USER_SETTINGS_HPP */
