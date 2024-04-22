

#include "box.hpp"

namespace widgets {

void box::update_layout() {
    if (box_kind == kind::HORIZONTAL) {
        box_height = 0;
        int current_width = 0;
        for (const auto &child : children) {
            box_height = std::max(child->height(), box_height);
            current_width += child->width();
        }
        box_width = current_width;
    } else {
        box_width = 0;
        int current_height = 0;
        for (const auto &child : children) {
            box_width = std::max(child->width(), box_width);
            current_height += child->height();
        }
        box_height = current_height;
    }
}

widget *box::child_at(int x, int y) {
    if (box_kind == kind::HORIZONTAL) {
        int current_width = 0;
        for (const auto &child : children) {
            if (current_width + child->width() > x) {
                return child->child_at(
                    x - current_width, y - (box_height - child->height()) / 2
                );
            }
            current_width += child->width();
        }
        return nullptr;
    } else {
        int current_height = 0;
        for (const auto &child : children) {
            if (current_height + child->height() > y) {
                return child->child_at(
                    x - (box_width - child->width()) / 2, y - current_height
                );
            }
            current_height += child->height();
        }
        return nullptr;
    }
}

widget *box::get(int index) const {
    return children[index].get();
}

widget *box::add(std::unique_ptr<widget> new_widget) {
    children.push_back(std::move(new_widget));
    update_layout();
    return children[size() - 1].get();
}

std::unique_ptr<widget> box::remove(int index) {
    auto it = children.begin() + index;
    std::unique_ptr<widget> removed_element = std::move(children[index]);
    children.erase(it);
    update_layout();
    return removed_element;
}

[[nodiscard]] int box::width() const {
    return box_width;
}

[[nodiscard]] int box::height() const {
    return box_height;
}

[[nodiscard]] std::unique_ptr<box> make_box(const box::kind &kind) {
    return std::make_unique<box>(kind);
}

}  // namespace widgets
