#pragma once

#include "FileView.hpp"
#include "Message.hpp"
#include "TextInput.hpp"
#include <filesystem>
#include <ncurses.h>

namespace tfc {

    class App {
        std::filesystem::path cd;
        FileView view;
        TextInput text_input;
        Message message;

        FILE *tty_file;
        SCREEN *term_screen;
        bool is_initialized = false;

        bool check_for_current();

        void back_dir();

        void enter_dir(std::filesystem::path to);

    public:
        explicit App(const std::filesystem::path &cd, const std::string &name);

        ~App();

        std::string run();
    };

}
