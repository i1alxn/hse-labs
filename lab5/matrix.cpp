#include "matrix.hpp"

namespace matrix_interpreter {

matrix &matrix::operator+=(const matrix &other) {
    check_dimension_mismatch(data.size(), other.data.size());
    if (!data.empty()) {
        check_dimension_mismatch(data[0].size(), other.data[0].size());
    }
    for (size_t row = 0; row < data.size(); ++row) {
        for (size_t column = 0; column < data[0].size(); ++column) {
            data[row][column] += other.data[row][column];
        }
    }

    return *this;
}

matrix &matrix::operator*=(const matrix &other) {
    if (data.empty()) {
        check_dimension_mismatch(0, other.data.size());
    }
    if (data.empty() && other.data.empty()) {
        data = std::vector<std::vector<long long>>();
        return *this;
    }
    check_dimension_mismatch(data[0].size(), other.data.size());
    std::vector<std::vector<long long>> result(
        data.size(), std::vector<long long>(other.data[0].size())
    );
    for (size_t row = 0; row < data.size(); ++row) {
        for (size_t column = 0; column < other.data[0].size(); ++column) {
            for (size_t i = 0; i < data[0].size(); ++i) {
                result[row][column] += data[row][i] * other.data[i][column];
            }
        }
    }
    data = std::move(result);
    return *this;
}

[[nodiscard]] long long
matrix::get(unsigned long long row, unsigned long long column) const {
    return data.at(row).at(column);
}

[[nodiscard]] size_t matrix::get_rows() const noexcept {
    return data.size();
}

[[nodiscard]] size_t matrix::get_columns() const noexcept {
    return data.empty() ? 0 : data[0].size();
}

}  // namespace matrix_interpreter
