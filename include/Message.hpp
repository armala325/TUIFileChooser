#pragma once

#include <string>

namespace tfc {

    class Message {
        std::string text;
        bool _is_error;

    public:
        Message() : _is_error(false) {}
        Message(const std::string &text) : text(text), _is_error(false) {}
        Message(const char *text) : text(text), _is_error(false) {}

        const std::string &get_text() const {
            return text;
        }

        const char *c_str() const {
            return text.c_str();
        }

        operator std::string() const {
            return get_text();
        }

        operator const char *() const {
            return c_str();
        }

        bool is_error() const {
            return _is_error;
        }

        bool empty() const {
            return text.empty();
        }

        operator bool() const {
            return !empty();
        }

        void clear() {
            text.clear();
            _is_error = false;
        }

        void set_error(bool is_error = true) {
            _is_error = is_error;
        }
    };

}
