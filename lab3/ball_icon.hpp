#ifndef BALL_ICON_HPP_
#define BALL_ICON_HPP_

#include <memory>
#include <string>
#include "abstract_widgets.hpp"

namespace widgets {
struct ball_icon final : widget {
    [[nodiscard]] int width() const final;

    [[nodiscard]] int height() const final;

    ball_icon() = default;

    explicit ball_icon(int ball_radius);

    [[nodiscard]] int radius() const;

    void radius(int new_radius);

    widget *child_at(int x, int y) final;

private:
    int ball_radius{};
};

[[nodiscard]] std::unique_ptr<ball_icon> make_ball_icon(int new_radius);
}  // namespace widgets

#endif  // BALL_ICON_HPP_
