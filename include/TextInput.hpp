#pragma once

#include <string>

namespace tfc {

    class TextInput {
    public:
        enum class Type {
            INACTIVE,

            SEARCH,
            NEW_FILE,
            NEW_DIR,
        };

    private:
        std::string text;
        Type type = Type::INACTIVE;
        bool is_modified = false;

    public:
        explicit TextInput() = default;

        const std::string &get_text() const {
            return text;
        }

        Type get_type() const {
            return type;
        }

        bool is_active() const {
            return type != Type::INACTIVE;
        }

        void deactivate() {
            text.clear();
            type = Type::INACTIVE;
            is_modified = false;
        }

        void activate(Type type) {
            this->type = type;
            is_modified = true;
        }

        void append(char c) {
            text += c;

            is_modified = true;
        }

        bool pop() {
            if (text.empty()) return false;

            text.pop_back();

            is_modified = true;
            return true;
        }

        bool check_modified() {
            bool out = is_modified;

            is_modified = false;
            return out;
        }
    };

}
