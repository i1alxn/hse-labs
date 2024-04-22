
#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace matrix_interpreter {

struct matrix_exception : std::runtime_error {
    explicit matrix_exception(const std::string &exception_text)
        : std::runtime_error(exception_text) {
    }
};

struct dimension_mismatch : matrix_exception {
    dimension_mismatch(size_t lhs, size_t rhs)
        : matrix_exception(
              "Dimension mismatch: lhs=" + std::to_string(lhs) +
              ", rhs=" + std::to_string(rhs)
          ) {
    }
};

struct matrix {
    matrix() = default;

    explicit matrix(std::vector<std::vector<long long>> &&data)
        : data(std::move(data)) {
    }

    matrix &operator+=(const matrix &other);

    matrix &operator*=(const matrix &other);

    [[nodiscard]] long long
    get(unsigned long long row, unsigned long long column) const;

    [[nodiscard]] size_t get_rows() const noexcept;

    [[nodiscard]] size_t get_columns() const noexcept;

private:
    static void check_dimension_mismatch(size_t lhs, size_t rhs) {
        if (lhs != rhs) {
            throw dimension_mismatch(lhs, rhs);
        }
    }

    std::vector<std::vector<long long>> data;
};

}  // namespace matrix_interpreter