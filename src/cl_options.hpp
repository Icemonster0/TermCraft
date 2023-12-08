#ifndef CL_OPTIONS_HPP
#define CL_OPTIONS_HPP

#include <string>
#include <vector>
#include <cstdio>

struct Option {
    Option(std::string p_name, std::string p_value) : name(p_name), value(p_value) {
    }

    std::string name;
    std::string value;
};

class CL_Options {
public:
    CL_Options(int argc, char const *argv[], std::string hint = "Invalid command line arguments!") {
        if (argc % 2) {
            printf("%s\n", hint);
            exit(1);
        }
        
        for (int i = 1; i < argc-1; ++i) {
            options.emplace_back(argv[i], argv[i+1]);
        }
    }

    std::string get_option_value(std::string name) {
        for (Option &option : options) {
            if (option.name.compare(name) == 0) {
                return option.value;
            }
        }
        return {""};
    }

private:
    std::vector<Option> options;
};

#endif /* Nd of header guard CL_OPTIONS_HPP */
