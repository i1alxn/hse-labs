#include "abstract_widgets.hpp"

namespace widgets {
widget *widget::child_at(int x, int y) {
    if (x >= 0 && y >= 0 && x < width() && y < height()) {
        return this;
    } else {
        return nullptr;
    }
}
}  // namespace widgets
