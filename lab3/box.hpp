#ifndef BOX_HPP_
#define BOX_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "abstract_widgets.hpp"

namespace widgets {
struct box final : container {
    void update_layout() final;

    [[nodiscard]] int width() const final;

    [[nodiscard]] int height() const final;

    enum kind { HORIZONTAL, VERTICAL };

    widget *child_at(int x, int y) final;

    [[nodiscard]] int size() const {
        return static_cast<int>(children.size());
    }

    [[nodiscard]] widget *get(int index) const;

    widget *add(std::unique_ptr<widget> new_widget);

    std::unique_ptr<widget> remove(int index);

    explicit box(kind kind) : box_kind(kind){};

private:
    std::vector<std::unique_ptr<widget>> children;
    kind box_kind;
    int box_width = 0, box_height = 0;
};

[[nodiscard]] std::unique_ptr<box> make_box(const box::kind &kind);

}  // namespace widgets

#endif  // BOX_HPP_
