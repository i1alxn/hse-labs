#ifndef GRID_HPP_
#define GRID_HPP_

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "abstract_widgets.hpp"

namespace widgets {
struct grid final : container {
    void update_layout() final;

    [[nodiscard]] int width() const final;

    [[nodiscard]] int height() const final;

    widget *child_at(int x, int y) final;

    [[nodiscard]] int rows() const {
        return grid_rows;
    }

    [[nodiscard]] int columns() const {
        return grid_columns;
    }

    [[nodiscard]] widget *get(int row, int column) const;

    widget *add(std::unique_ptr<widget> new_widget, int row, int column);

    std::unique_ptr<widget> remove(int row, int column);

    explicit grid(int num_rows, int num_columns)
        : grid_rows(num_rows), grid_columns(num_columns) {
        children.resize(grid_rows);
        for (int i = 0; i < grid_rows; i++) {
            children[i].resize(grid_columns);
        }
        column_widths.resize(grid_columns);
        rows_heights.resize(grid_rows);
    }

private:
    std::vector<std::vector<std::unique_ptr<widget>>> children;
    int grid_rows, grid_columns;
    std::vector<int> column_widths;
    std::vector<int> rows_heights;
    int grid_width = 0;
    int grid_height = 0;
};

[[nodiscard]] std::unique_ptr<grid> make_grid(int num_rows, int num_columns);

}  // namespace widgets

#endif  // GRID_HPP_
