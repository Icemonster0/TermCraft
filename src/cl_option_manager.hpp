#ifndef CL_OPTION_MANAGER_HPP
#define CL_OPTION_MANAGER_HPP

#include <string>
#include <vector>
#include <iostream>

struct CLOM_Setting {
    CLOM_Setting(std::string p_name, std::string p_value) : name(p_name), value(p_value) {
    }

    std::string name;
    std::string value;
};

struct CLOM_Flag {
    CLOM_Flag(std::string p_name) : name(p_name), is_set(false) {
    }

    std::string name;
    bool is_set;
};

class CL_Option_Manager {
public:

    CL_Option_Manager() : user_hint("Invalid command line options!\n") {
    }

    void add_setting(std::string name, std::string default_value) {
        settings.emplace_back(name, default_value);
    }

    void add_flag(std::string name) {
        flags.emplace_back(name);
    }

    void process_cl_options(int argc, char const *argv[]) {
        for (size_t i = 1; i < argc;) {
            CLOM_Setting *setting = get_setting_by_name(argv[i]);
            CLOM_Flag *flag = get_flag_by_name(argv[i]);

            if (setting != nullptr && i < argc-1) {
                setting->value = argv[i+1];
                i += 2;
            }
            else if (flag != nullptr) {
                flag->is_set = true;
                i += 1;
            }
            else {
                std::cout << "Unknown option: " << argv[i] << '\n';
                print_user_hint();
                exit(1);
            }
        }
    }

    std::string get_setting_value(std::string name) {
        CLOM_Setting *setting = get_setting_by_name(name);

        if (setting != nullptr)
            return setting->value;
        else
            return "";
    }

    bool is_flag_set(std::string name) {
        CLOM_Flag *flag = get_flag_by_name(name);

        if (flag != nullptr)
            return flag->is_set;
        else
            return false;
    }

    void set_user_hint(std::string hint) {
        user_hint = hint;
    }

    void print_user_hint() {
        std::cout << user_hint << '\n';
    }

private:

    CLOM_Setting* get_setting_by_name(std::string name) {
        for (CLOM_Setting &setting : settings) {
            if (setting.name.compare(name) == 0) {
                return &setting;
            }
        }

        return nullptr;
    }

    CLOM_Flag* get_flag_by_name(std::string name) {
        for (CLOM_Flag &flag : flags) {
            if (flag.name.compare(name) == 0) {
                return &flag;
            }
        }

        return nullptr;
    }

    std::vector<CLOM_Setting> settings;
    std::vector<CLOM_Flag> flags;

    std::string user_hint;
};

#endif /* end of include guard: CL_OPTION_MANAGER_HPP */
