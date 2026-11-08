#include "App.hpp"
#include <exception>
#include <filesystem>
#include <iostream>
#include <ncurses.h>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    try {
        tfc::App app(argc < 3 ? fs::current_path() : argv[2], argc < 2 ? "Pick file" : argv[1]);

        std::string result = app.run();
        if (!result.empty()) {
            std::cout << result << std::endl;
        } else {
            return 1;
        }

        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
