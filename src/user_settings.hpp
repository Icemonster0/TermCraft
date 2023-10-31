#ifndef USER_SETTINGS_HPP
#define USER_SETTINGS_HPP

struct user_settings {
    bool cursor_invis = true;
};

/* This object needs to be declared in main.cpp */
extern user_settings U;

#endif /* end of include guard: USER_SETTINGS_HPP */
