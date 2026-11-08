#pragma once

#include <algorithm>
#include <filesystem>
#include <vector>

namespace tfc {

    class FileView {
        std::vector<std::filesystem::path> files;
        size_t frame_top, frame_height, current;

    public:
        explicit FileView(const std::filesystem::path &dir, size_t frame_height) : frame_height(frame_height) {
            refresh(dir);
        }

        size_t get_frame_height() const {
            return frame_height;
        }

        size_t line_count() const {
            return frame_top < files.size() ? files.size() - frame_top : 0;
        }

        bool is_current(size_t i) const {
            return i + frame_top == current;
        }

        bool has_current() const {
            return current < files.size();
        }

        const std::filesystem::path &get_current() const {
            return files[current];
        }

        const std::filesystem::path &get(size_t i) const {
            return files[frame_top + i];
        }

        const std::filesystem::path &operator[](size_t i) const {
            return get(i);
        }

        void set_frame_height(size_t frame_height) {
            this->frame_height = frame_height;
        }

        void center_on_current() {
            if (current < frame_height / 2) {
                frame_top = 0;
            } else {
                frame_top = current - frame_height / 2;
            }
        }

        int refresh(const std::filesystem::path &dir);

        bool next(size_t multiplier = 1);

        bool prev(size_t multiplier = 1);

        bool select(const std::string &filename);

        auto begin() const {
            return files.begin() + frame_top;
        }

        auto end() const {
            return std::min(begin() + frame_height, files.end());
        }
    };

}
