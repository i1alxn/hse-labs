#include "ball_icon.hpp"

namespace widgets {

[[nodiscard]] int ball_icon::width() const {
    return 2 * ball_radius + 1;
}

[[nodiscard]] int ball_icon::height() const {
    return 2 * ball_radius + 1;
}

ball_icon::ball_icon(int ball_radius) : ball_radius(ball_radius) {
}

[[nodiscard]] int ball_icon::radius() const {
    return ball_radius;
}

void ball_icon::radius(int new_radius) {
    ball_radius = new_radius;
}

widget *ball_icon::child_at(int x, int y) {
    if ((ball_radius - x) * (ball_radius - x) +
            (ball_radius - y) * (ball_radius - y) <=
        ball_radius * ball_radius) {
        return this;
    } else {
        return nullptr;
    }
}

[[nodiscard]] std::unique_ptr<ball_icon> make_ball_icon(int new_radius) {
    return std::make_unique<ball_icon>(new_radius);
}
}  // namespace widgets
