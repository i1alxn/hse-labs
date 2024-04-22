#include "grid.hpp"
#include <cstring>

namespace widgets {

void grid::update_layout() {
    memset(column_widths.data(), 0, sizeof(int) * column_widths.size());
    memset(rows_heights.data(), 0, sizeof(int) * rows_heights.size());

    for (int i = 0; i < grid_rows; i++) {
        for (int j = 0; j < grid_columns; j++) {
            if (children[i][j] != nullptr) {
                column_widths[j] =
                    std::max(children[i][j]->width(), column_widths[j]);
                rows_heights[i] =
                    std::max(children[i][j]->height(), rows_heights[i]);
            }
        }
    }
    grid_width = 0;
    grid_height = 0;
    for (int i = 0; i < grid_columns; i++) {
        grid_width += column_widths[i];
    }
    for (int i = 0; i < grid_rows; i++) {
        grid_height += rows_heights[i];
    }
}

widget *grid::child_at(int x, int y) {
    int current_width = 0;
    int current_height = 0;
    int row = 0;
    int column = 0;
    for (; row < grid_rows; row++) {
        if (current_height + rows_heights[row] > y) {
            break;
        }
        current_height += rows_heights[row];
    }
    for (; column < grid_rows; column++) {
        if (current_width + column_widths[column] > x) {
            break;
        }
        current_width += column_widths[column];
    }
    if (row == grid_rows || column == grid_columns) {
        return nullptr;
    }
    if (children[row][column] != nullptr) {
        return children[row][column]->child_at(
            x - current_width, y - current_height
        );
    }
    return nullptr;
}

widget *grid::get(int row, int column) const {
    return children[row][column].get();
}

widget *grid::add(std::unique_ptr<widget> new_widget, int row, int column) {
    children[row][column] = std::move(new_widget);
    update_layout();
    return children[row][column].get();
}

std::unique_ptr<widget> grid::remove(int row, int column) {
    std::unique_ptr<widget> removed_element = std::move(children[row][column]);
    children[row][column] = nullptr;
    update_layout();
    return removed_element;
}

[[nodiscard]] int grid::width() const {
    return grid_width;
}

[[nodiscard]] int grid::height() const {
    return grid_height;
}

[[nodiscard]] std::unique_ptr<grid> make_grid(int num_rows, int num_columns) {
    return std::make_unique<grid>(num_rows, num_columns);
}

}  // namespace widgets
