#include "engine.hpp"

#include <cstdio>

int main(int argc, char const *argv[]) {

    tc::Engine engine {80, 24, 24}; // X res, Y res, target fps
    int result = engine.run();
    printf("Engine exited with return value %d\n", result);

    return 0;
}
