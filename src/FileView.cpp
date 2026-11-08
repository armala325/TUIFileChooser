#include "FileView.hpp"

int tfc::FileView::refresh(const std::filesystem::path &dir) {
    try {
        std::vector<std::filesystem::path> new_files;
        for (const auto &entry : std::filesystem::directory_iterator(dir)) {
            new_files.push_back(entry.path());
        }

        files = std::move(new_files);
    } catch (const std::filesystem::filesystem_error &e) {
        return -1;
    }

    std::sort(files.begin(), files.end(), [](const std::filesystem::path &a, const std::filesystem::path &b) {
        std::string a_filename = a.filename();
        std::string b_filename = b.filename();
        return std::lexicographical_compare(a_filename.begin(), a_filename.end(), b_filename.begin(), b_filename.end(), [](unsigned char a, unsigned char b) {
            return std::tolower(a) < std::tolower(b);
        });
    });

    frame_top = 0;
    current = 0;

    return 0;
}

bool tfc::FileView::next(size_t multiplier) {
    if (files.empty()) return false;

    bool out;
    size_t new_current = current + multiplier;
    if (new_current >= files.size()) {
        current = files.size() - 1;
        out = false;
    } else {
        current = new_current;
        out = true;
    }

    if (current - frame_top >= frame_height) {
        center_on_current();
    }

    return out;
}

bool tfc::FileView::prev(size_t multiplier) {
    if (files.empty()) return false;

    bool out;
    if (multiplier > current) {
        current = 0;
        out = false;
    } else {
        current -= multiplier;
        out = true;
    }

    if (current < frame_top) {
        center_on_current();
    }

    return out;
}

bool tfc::FileView::select(const std::string &filename) {
    size_t i;
    for (i = 0; i < files.size(); i++) {
        if (std::ranges::starts_with(files[i].filename().string(), filename, [](unsigned char a, unsigned char b) { return std::tolower(a) == std::tolower(b); })) {
            current = i;
            center_on_current();

            break;
        }
    }

    return i < files.size();
}
