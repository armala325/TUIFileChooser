#include "App.hpp"
#include "FileView.hpp"

#include <fstream>
#include <ncurses.h>
#include <stdexcept>

namespace fs = std::filesystem;

namespace {

    enum class ColorID : int {
        FILE = 1,
        DIR,
        OTHER,

        MESSAGE,
        ERROR,
    };

    fs::path normalize(const fs::path &path) {
        fs::path normal = path.lexically_normal();

        if (normal.filename().empty() && normal != normal.root_path()) {
            normal = normal.parent_path();
        }

        return normal;
    }

}

bool tfc::App::check_for_current() {
    if (view.has_current()) {
        return true;
    } else {
        message = "No files";

        return false;
    }
}

void tfc::App::back_dir() {
    if (cd.has_parent_path()) {
        std::filesystem::path parent = cd.parent_path();

        if (view.refresh(parent) == 0) {
            cd = parent;
        } else {
            message = "Failed to read parent directory";
            message.set_error();
        }
    } else {
        message = "No parent directory";
    }

    text_input.deactivate();
}

void tfc::App::enter_dir(std::filesystem::path to) {
    if (std::filesystem::is_directory(to)) {
        if (view.refresh(to) == 0) {
            cd = std::filesystem::absolute(to);
        } else {
            message = "Failed to read directory";
            message.set_error();
        }
    } else {
        message = "Not a directory";
    }

    text_input.deactivate();
}

tfc::App::App(const fs::path &cd, const std::string &name) : cd(normalize(fs::absolute(cd))), view(cd, 1), message(name) {
    tty_file = fopen("/dev/tty", "r+");
    if (!tty_file) {
        throw std::runtime_error("Error: Failed to open file '/dev/tty'");
    }

    term_screen = newterm(NULL, tty_file, tty_file);
    if (!term_screen) {
        fclose(tty_file);
        throw std::runtime_error("Error: Failed to create terminal screen");
    }
    set_term(term_screen);

    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    set_escdelay(50);

    if (has_colors()) {
        start_color();

        init_pair(static_cast<int>(ColorID::FILE), COLOR_WHITE, COLOR_BLACK);
        init_pair(static_cast<int>(ColorID::DIR), COLOR_BLUE, COLOR_BLACK);
        init_pair(static_cast<int>(ColorID::OTHER), COLOR_YELLOW, COLOR_BLACK);
        init_pair(static_cast<int>(ColorID::MESSAGE), COLOR_BLACK, COLOR_WHITE);
        init_pair(static_cast<int>(ColorID::ERROR), COLOR_RED, COLOR_WHITE);
    }

    view.set_frame_height(LINES - 1);

    is_initialized = true;
}

tfc::App::~App() {
    if (is_initialized) {
        endwin();
        delscreen(term_screen);
        fclose(tty_file);

        is_initialized = false;
    }
}

std::string tfc::App::run() {
    while (1) {
        clear();

        attrset(A_BOLD | COLOR_PAIR(static_cast<int>((message.is_error() ? ColorID::ERROR : ColorID::MESSAGE))));
        addnstr(!message.empty() ? message.c_str() : cd.c_str(), COLS - 1);
        hline(' ', COLS);
        move(1, 0);

        for (size_t i = 0; i < view.get_frame_height() && i < view.line_count(); i++) {
            const std::filesystem::path &file = view[i];

            ColorID color;
            if (std::filesystem::is_regular_file(file)) {
                color = ColorID::FILE;
            } else if (std::filesystem::is_directory(file)) {
                color = ColorID::DIR;
            } else {
                color = ColorID::OTHER;
            }

            attrset((std::filesystem::is_symlink(file) ? A_ITALIC : 0) | (view.is_current(i) ? A_REVERSE : 0) | COLOR_PAIR(static_cast<int>(color)));
            addnstr(file.filename().c_str(), COLS - 1);
            addch('\n');
        }

        refresh();

        int c = getch();
        message.clear();
        switch (c) {
        case KEY_EXIT: {
            return "";

            break;
        } case 27: {
            if (text_input.is_active()) {
                text_input.deactivate();
            } else {
                return "";
            }

            break;
        }

        case KEY_BACKSPACE: {
            if (text_input.is_active()) {
                if (!text_input.get_text().empty()) {
                    text_input.pop();
                } else {
                    text_input.deactivate();
                }
            } else {
                back_dir();
            }

            break;
        } case KEY_LEFT: {
            back_dir();

            break;
        } case '\n': {
            if (!text_input.is_active() || text_input.get_type() == TextInput::Type::SEARCH) {
                if (check_for_current()) {
                    enter_dir(view.get_current());
                }
            } else if (text_input.get_type() == TextInput::Type::NEW_FILE) {
                const std::filesystem::path &path = cd / text_input.get_text();

                if (!std::filesystem::exists(path)) {
                    std::ofstream file(path);
                    if (!file) {
                        message = "Failed to open file";
                        message.set_error();
                    } else {
                        file.close();
                        if (view.refresh(cd) != 0) {
                            message = "Failed to reload current directory (file still created)";
                            message.set_error();
                        }
                    }
                } else {
                    message = "File already exists";
                }

                view.select(path.filename());
            } else if (text_input.get_type() == TextInput::Type::NEW_DIR) {
                const std::filesystem::path &path = cd / text_input.get_text();

                if (!std::filesystem::exists(path)) {
                    if (!std::filesystem::create_directory(path)) {
                        message = "Failed to create directory";
                        message.set_error();
                    } else {
                        if (view.refresh(cd) != 0) {
                            message = "Failed to reload current directory (directory still created)";
                            message.set_error();
                        }
                    }
                } else {
                    message = "File already exists";
                }

                view.select(path.filename());
            }

            text_input.deactivate();

            break;
        } case KEY_RIGHT: {
            if (check_for_current()) {
                enter_dir(view.get_current());
            }

            break;
        } case KEY_UP: {
            if (!view.prev()) {
                message = "No files above";
            }

            break;
        } case KEY_DOWN: {
            if (!view.next()) {
                message = "No files below";
            }

            break;
        }

        case '?': {
            if (text_input.is_active()) {
                text_input.append('?');
            } else {
                text_input.activate(TextInput::Type::SEARCH);
                message = "Search: ''";
            }

            break;
        }

        case '=': {
            if (text_input.is_active()) {
                text_input.append(c);
            } else {
                text_input.activate(TextInput::Type::NEW_FILE);
            }

            break;
        } case '+': {
            if (text_input.is_active()) {
                text_input.append(c);
            } else {
                text_input.activate(TextInput::Type::NEW_DIR);
            }

            break;
        }

        case ' ': {
            if (text_input.is_active()) {
                text_input.append(' ');
            } else {
                if (check_for_current()) {
                    return std::filesystem::absolute(view.get_current());
                }
            }

            break;
        }

        case KEY_RESIZE: {
            view.set_frame_height(LINES - 1);

            break;
        }

        default: {
            if (c >= 32 && c <=126) {
                if (!text_input.is_active()) {
                    text_input.activate(TextInput::Type::SEARCH);
                }

                text_input.append(c);
            }

            break;
        }
        }

        if (text_input.check_modified()) {
            if (text_input.get_type() == TextInput::Type::SEARCH) {
                if (!view.select(text_input.get_text())) {
                    message = std::format("No matches found for '{}'", text_input.get_text());
                } else {
                    message = std::format("Search: '{}'", text_input.get_text());
                }
            } else if (text_input.get_type() == TextInput::Type::NEW_FILE) {
                message = std::format("New file: '{}'", text_input.get_text());
            } else if (text_input.get_type() == TextInput::Type::NEW_DIR) {
                message = std::format("New directory: '{}'", text_input.get_text());
            }
        }
    }
}
