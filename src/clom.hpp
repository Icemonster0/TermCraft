/*
* CLOM is licensed under the MIT License:
*
* Copyright (c) 2023 András Borsányi
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#ifndef CLOM_HPP
#define CLOM_HPP

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <typeinfo>
#include <memory>
#include <cxxabi.h>
#include <algorithm>

class CL_Option_Manager {
private:

    enum CLOM_Type {
        INT, FLOAT, DOUBLE, CHAR, STRING, N_A
    };

    static const inline std::string clom_type_names[5] {
        "int", "float", "double", "char", "std::string"
    };

    struct CLOM_Option {
    public:
        CLOM_Option(std::string p_name, CLOM_Type p_type, std::string p_description)
            : name(p_name), type(p_type), description(p_description) {
        }

        // this is not hacky idk what you're talking about
        virtual void this_is_polymorphic() = 0;

        std::string get_name() {return name;}
        CLOM_Type get_type() {return type;}
        std::string get_description() {return description;}

    protected:
        std::string name;
        CLOM_Type type;
        std::string description;
    };

    template<typename T>
    struct CLOM_Setting : public CLOM_Option {
        CLOM_Setting(std::string p_name, T p_value, std::string p_description)
            : CLOM_Option {p_name, CL_Option_Manager::cpp_type_to_clom_type<T>(), p_description}, value(p_value) {
        }

        // the other bit that is also not hacky
        void this_is_polymorphic() {}

        void set_value(T v) {value = v;}
        T get_value() {return value;}

    private:
        T value;
    };

    struct CLOM_Flag : CLOM_Option {
        CLOM_Flag(std::string p_name, std::string p_description)
            : CLOM_Option {p_name, N_A, p_description}, is_set(false) {
        }

        // no hackage going on here
        void this_is_polymorphic() {}

        void set_is_set(bool v) {is_set = v;}
        bool get_is_set() {return is_set;}

    private:
        bool is_set;
    };

public:

    CL_Option_Manager() {
    }

    template<typename T>
    void register_setting(std::string name, T default_value, std::string description = "") {
        CLOM_Type type = cpp_type_to_clom_type<T>();
        if (get_general_setting_by_name(name) || get_flag_by_name(name)) {
            std::cout << "error: Option '" << name << "' may only be registered once!\n"
                      << "(from: CL_Option_Manager::register_setting<"
                      << clom_type_names[type]
                      << ">(\"" << name << "\", " << default_value << "))" << '\n';
            exit(1);
        }
        settings.push_back(std::unique_ptr<CLOM_Option>(new CLOM_Setting<T>(name, default_value, description)));
    }

    void register_flag(std::string name, std::string description = "") {
        if (get_general_setting_by_name(name) || get_flag_by_name(name)) {
            std::cout << "error: Option '" << name << "' may only be registered once!\n"
                      << "(from: CL_Option_Manager::register_flag"
                      << "(\"" << name << "\"))" << '\n';
            exit(1);
        }
        flags.emplace_back(name, description);
    }

    void process_cl_options(int argc, char const *argv[]) {
        for (size_t i = 1; i < argc;) {
            CLOM_Option *setting = get_general_setting_by_name(argv[i]);
            CLOM_Flag *flag = get_flag_by_name(argv[i]);

            if (setting) {
                if (!(i < argc-1)) {
                    std::cout << "error: Missing value for setting '" << argv[i] << "'\n";
                    print_user_hint();
                    exit(1);
                }

                try {
                    switch (setting->get_type()) {
                        case INT: dynamic_cast<CLOM_Setting<int>*>(setting)->set_value(std::stoi(argv[i+1])); break;
                        case FLOAT: dynamic_cast<CLOM_Setting<float>*>(setting)->set_value(std::stof(argv[i+1])); break;
                        case DOUBLE: dynamic_cast<CLOM_Setting<double>*>(setting)->set_value(std::stod(argv[i+1])); break;
                        case CHAR: dynamic_cast<CLOM_Setting<char>*>(setting)->set_value(argv[i+1][0]); break;
                        case STRING: dynamic_cast<CLOM_Setting<std::string>*>(setting)->set_value(argv[i+1]); break;
                    }
                } catch (...) {
                    std::cout << "error: Failed to parse value '" << argv[i+1] << "' of setting '" << argv[i]
                              << "'! Expected type: " << clom_type_names[setting->get_type()] << "\n";
                    print_user_hint();
                    exit(1);
                }

                i += 2;
            }

            else if (flag) {
                flag->set_is_set(true);
                i += 1;
            }

            else {
                std::cout << "error: Unknown option: '" << argv[i] << "'\n";
                print_user_hint();
                exit(1);
            }
        }
    }

    template<typename T>
    T get_setting_value(std::string name) {
        CLOM_Option *setting = get_general_setting_by_name(name);

        CLOM_Type t = cpp_type_to_clom_type<T>();
        if (setting) {

            if (setting->get_type() != t) {
                std::cout << "error: Type of '" << name << "' is " << clom_type_names[setting->get_type()]
                          << ", but " << clom_type_names[t]
                          << " requested\n(from: CL_Option_Manager::get_setting_value<" << clom_type_names[t] << ">(\"" << name << "\"))\n";
                exit(1);
            }

            return dynamic_cast<CLOM_Setting<T>*>(setting)->get_value();
        }
        else {
            std::cout << "error: The setting '" << name << "' is not registered!\n"
                      << "(from: CL_Option_Manager::get_setting_value<" << clom_type_names[t] << ">(\"" << name << "\"))\n";
            exit(1);
        }
    }

    bool is_flag_set(std::string name) {
        CLOM_Flag *flag = get_flag_by_name(name);

        if (flag)
            return flag->get_is_set();
        else {
            std::cout << "error: The flag '" << name << "' is not registered!\n"
                      << "(from: CL_Option_Manager::is_flag_set(\"" << name << "\"))\n";
            exit(1);
        }
    }

    void set_user_hint(std::string hint) {
        user_hint = hint;
    }

    std::string get_user_hint() {
        return user_hint;
    }

    void generate_user_hint(std::string app_name) {
        std::sort(settings.begin(), settings.end(), [](std::unique_ptr<CLOM_Option> &a, std::unique_ptr<CLOM_Option> &b) {
            return a->get_name().compare(b->get_name()) < 0;
        });
        std::sort(flags.begin(), flags.end(), [](CLOM_Flag a, CLOM_Flag b) {
            return a.get_name().compare(b.get_name()) < 0;
        });

        std::stringstream ss;

        ss << "Usage: " << app_name << " [<setting> <value>] [<flag>] ...\n";
        ss << "\n";

        const int window_w = 80;
        const int name_w = 20;
        const int type_w = 16;
        const int desc_w = window_w-name_w-type_w;

        int column = 0;

        ss << "Settings:\n";
        ss << std::left << std::setw(name_w) << " Name:";
        ss << std::left << std::setw(type_w) << " Type:";
        ss << " Description:\n";

        for (int i = 0; i < settings.size(); ++i) {
            ss << "  " << std::left << std::setw(name_w-2) << settings[i]->get_name();
            if (settings[i]->get_name().length() + 2 > name_w)
                ss << "\n" << std::setw(name_w) << " ";

            ss << "  " << std::left << std::setw(type_w-2) << clom_type_names[settings[i]->get_type()];
            if (clom_type_names[settings[i]->get_type()].length() + 2 > type_w)
                ss << "\n" << std::setw(name_w+type_w) << " ";

            ss << "  ";
            int column = 0;
            for (char c : settings[i]->get_description()) {
                if (column >= desc_w-2 || c == '\n') {
                    ss << "\n" << std::setw(name_w+type_w+2) << " ";
                    column = 0;
                }
                if (c != '\n') {
                    ++column;
                    ss << c;
                }
            }

            ss << "\n";
        }

        ss << "\n";

        ss << "Flags:\n";
        ss << std::left << std::setw(name_w) << " Name:";
        ss << " Description:\n";

        for (int i = 0; i < flags.size(); ++i) {
            ss << "  " << std::left << std::setw(name_w-2) << flags[i].get_name();
            if (flags[i].get_name().length() + 2 > name_w)
                ss << "\n" << std::setw(name_w) << " ";

            ss << "  ";
            int column = 0;
            for (char c : flags[i].get_description()) {
                if (column >= desc_w+type_w-2 || c == '\n') {
                    ss << "\n" << std::setw(name_w+2) << " ";
                    column = 0;
                }
                if (c != '\n') {
                    ++column;
                    ss << c;
                }
            }

            ss << "\n";
        }

        user_hint = ss.str();
    }

    void print_user_hint() {
        std::cout << user_hint;
        if (user_hint[user_hint.length()-1] != '\n') std::cout << '\n';
    }

private:

    CLOM_Option* get_general_setting_by_name(std::string name) {
        for (std::unique_ptr<CLOM_Option> &setting : settings) {
            if (setting->get_name().compare(name) == 0) {
                return setting.get();
            }
        }

        return nullptr;
    }

    CLOM_Flag* get_flag_by_name(std::string name) {
        for (CLOM_Flag &flag : flags) {
            if (flag.get_name().compare(name) == 0) {
                return &flag;
            }
        }

        return nullptr;
    }

    template<typename T>
    static CLOM_Type cpp_type_to_clom_type() {
        if      (typeid(T) == typeid(int)) return INT;
        else if (typeid(T) == typeid(float)) return FLOAT;
        else if (typeid(T) == typeid(double)) return DOUBLE;
        else if (typeid(T) == typeid(char)) return CHAR;
        else if (typeid(T) == typeid(std::string)) return STRING;
        // THERE CAN BE NO RETURN STATEMENT RETURNING 'CLOM_Type::N_A'!
        else {
            int status;
            char *realname = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, &status);
            std::cout << "error: Unsopported setting type: " << realname << "\n"
                      << "(from: CL_Option_Manager::cpp_type_to_clom_type<" << realname << ">())\n"
                      << "valid types are: ";
            for (std::string type_name : clom_type_names) std::cout << type_name << "; ";
            std::cout << '\n';
            std::free(realname);
            exit(1);
        }
    }

    std::vector<std::unique_ptr<CLOM_Option>> settings;
    std::vector<CLOM_Flag> flags;

    std::string user_hint = "";
};

#endif /* end of include guard: CLOM_HPP */
