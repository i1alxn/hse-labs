#ifndef VECTOR_HPP_
#define VECTOR_HPP_

#include <exception>
#include <memory>
#include <stdexcept>
#include <utility>
#include "vector_config.hpp"

namespace lab_vector_naive {
template <typename T, typename Alloc = std::allocator<T>>
class vector {
    size_t m_capacity = 0;
    size_t m_size = 0;
    T *m_data = nullptr;

    void check_index_is_in_range(size_t index) const {
        if (index >= m_size) {
            throw std::out_of_range("Index is out of range");
        }
    }

    void reset_size_and_capacity_and_deallocate_memory() VECTOR_NOEXCEPT {
        Alloc().deallocate(m_data, m_capacity);
        m_size = 0;
        m_capacity = 0;
    }

    void destruct_redundant_elements(size_t new_size) VECTOR_NOEXCEPT {
        for (size_t i = new_size; i < m_size; ++i) {
            m_data[i].~T();
        }
    }

    void decrease_capacity(size_t new_capacity) VECTOR_NOEXCEPT {
        if (new_capacity == m_capacity) {
            return;
        }
        Alloc().deallocate(m_data + new_capacity, m_capacity - new_capacity);
        m_capacity = new_capacity;
    }

    void swap(vector &&other) VECTOR_NOEXCEPT {
        std::swap(m_data, other.m_data);
        std::swap(m_size, other.m_size);
        std::swap(m_capacity, other.m_capacity);
    }

    void increase_capacity(size_t new_size) {
        if (new_size <= m_capacity) {
            return;
        }
        size_t new_capacity = round_up_to_the_power_of_two(new_size);
        T *new_data = Alloc().allocate(new_capacity);
        try {
            for (size_t i = 0; i < m_size; ++i) {
                new (new_data + i) T(std::move(m_data[i]));
                m_data[i].~T();
            }
            Alloc().deallocate(m_data, m_capacity);
            m_data = new_data;
            m_capacity = new_capacity;
        } catch (...) {
            Alloc().deallocate(new_data, new_capacity);
            throw;
        }
    }

    size_t round_up_to_the_power_of_two(size_t x) VECTOR_NOEXCEPT {
        if (x == 0) {
            return 0;
        }
        size_t res = 1;
        while (res < x) {
            res <<= 1;
        }
        return res;
    }

public:
    vector() = default;

    explicit vector(size_t size)
        : m_capacity(round_up_to_the_power_of_two(size)),
          m_size(size),
          m_data(Alloc().allocate(m_capacity)) {
        try {
            for (size_t i = 0; i < size; ++i) {
                new (m_data + i) T();
            }
        } catch (...) {
            reset_size_and_capacity_and_deallocate_memory();
            throw;
        }
    }

    vector(size_t size, const T &value)
        : m_capacity(round_up_to_the_power_of_two(size)),
          m_size(size),
          m_data(Alloc().allocate(m_capacity)) {
        try {
            for (size_t i = 0; i < size; ++i) {
                new (m_data + i) T(value);
            }
        } catch (...) {
            reset_size_and_capacity_and_deallocate_memory();
            throw;
        }
    }

    vector(size_t size, T &&value)
        : m_capacity(round_up_to_the_power_of_two(size)),
          m_size(size),
          m_data(Alloc().allocate(m_capacity)) {
        try {
            if (size > 1) {
                for (size_t i = 0; i < size; ++i) {
                    new (m_data + i) T(value);
                }
            } else {
                new (m_data) T(std::move(value));
            }
        } catch (...) {
            reset_size_and_capacity_and_deallocate_memory();
            throw;
        }
    }

    vector(const vector &other)
        : m_capacity(round_up_to_the_power_of_two(other.m_size)),
          m_size(other.m_size),
          m_data(Alloc().allocate(m_capacity)) {
        try {
            for (size_t i = 0; i < m_size; ++i) {
                new (m_data + i) T(other.m_data[i]);
            }
        } catch (...) {
            reset_size_and_capacity_and_deallocate_memory();
            throw;
        }
    }

    vector(vector &&other) VECTOR_NOEXCEPT
        : m_capacity(std::exchange(other.m_capacity, 0)),
          m_size(std::exchange(other.m_size, 0)),
          m_data(std::exchange(other.m_data, nullptr)) {
    }

    vector &operator=(vector &&other) VECTOR_NOEXCEPT {
        if (this != &other) {
            clear();
            swap(std::move(other));
            decrease_capacity(round_up_to_the_power_of_two(m_size));
        }
        return *this;
    }

    vector &operator=(const vector &other) {
        if (this != &other) {
            if (m_size < other.m_size) {
                vector tmp(other);
                *this = std::move(tmp);
                decrease_capacity(round_up_to_the_power_of_two(m_size));
            } else {
                for (size_t i = 0; i < other.m_size; ++i) {
                    m_data[i] = other.m_data[i];
                }
                destruct_redundant_elements(other.m_size);
                m_size = other.m_size;
            }
        }
        return *this;
    }

    ~vector() VECTOR_NOEXCEPT {
        if (m_data != nullptr) {
            clear();
            Alloc().deallocate(m_data, m_capacity);
        }
    }

    [[nodiscard]] size_t size() const VECTOR_NOEXCEPT {
        return m_size;
    }

    [[nodiscard]] size_t capacity() const VECTOR_NOEXCEPT {
        return m_capacity;
    }

    [[nodiscard]] bool empty() const VECTOR_NOEXCEPT {
        return m_size == 0;
    }

    void clear() VECTOR_NOEXCEPT {
        for (size_t i = 0; i < m_size; ++i) {
            m_data[i].~T();
        }
        m_size = 0;
    }

    void push_back(const T &value) {
        if (m_size + 1 <= m_capacity) {
            new (m_data + m_size) T(value);
        } else {
            size_t new_capacity = round_up_to_the_power_of_two(m_size + 1);
            T *new_data = Alloc().allocate(new_capacity);
            try {
                new (new_data + m_size) T(value);
                for (size_t i = 0; i < m_size; ++i) {
                    new (new_data + i) T(std::move(m_data[i]));
                    m_data[i].~T();
                }
                Alloc().deallocate(m_data, m_capacity);
                m_data = new_data;
                m_capacity = new_capacity;
            } catch (...) {
                Alloc().deallocate(new_data, new_capacity);
                throw;
            }
        }
        ++m_size;
    }

    void push_back(T &&value) {
        increase_capacity(m_size + 1);
        new (m_data + m_size) T(std::move(value));
        ++m_size;
    }

    void pop_back() {
        m_data[m_size - 1].~T();
        --m_size;
    }

    void resize(size_t new_size) {
        destruct_redundant_elements(new_size);

        size_t old_capacity = m_capacity;
        increase_capacity(new_size);
        try {
            for (size_t i = m_size; i < new_size; ++i) {
                new (m_data + i) T();
            }
        } catch (...) {
            decrease_capacity(old_capacity);
            throw;
        }
        m_size = new_size;
    }

    void resize(size_t new_size, const T &new_value) {
        destruct_redundant_elements(new_size);
        if (new_size <= m_capacity) {
            for (size_t i = m_size; i < new_size; ++i) {
                new (m_data + i) T(new_value);
            }
        } else {
            size_t new_capacity = round_up_to_the_power_of_two(new_size);
            T *new_data = Alloc().allocate(new_capacity);
            try {
                new (new_data + m_size) T(new_value);
                for (size_t i = 0; i < m_size; ++i) {
                    new (new_data + i) T(std::move(m_data[i]));
                    m_data[i].~T();
                }
                Alloc().deallocate(m_data, m_capacity);
                for (size_t i = m_size + 1; i < new_size; ++i) {
                    new (new_data + i) T(new_data[m_size]);
                }
                m_data = new_data;
                m_capacity = new_capacity;
            } catch (...) {
                Alloc().deallocate(new_data, new_capacity);
                throw;
            }
        }
        m_size = new_size;
    }

    void resize(size_t new_size, T &&new_value) {
        destruct_redundant_elements(new_size);
        size_t old_capacity = m_capacity;
        increase_capacity(new_size);
        try {
            if (new_size > 1) {
                for (size_t i = m_size; i < new_size; ++i) {
                    new (m_data + i) T(new_value);
                }
            } else {
                new (m_data) T(std::move(new_value));
            }
        } catch (...) {
            decrease_capacity(old_capacity);
            throw;
        }
        m_size = new_size;
    }

    T &operator[](std::size_t index) & {
        return m_data[index];
    }

    const T &operator[](std::size_t index) const & {
        return m_data[index];
    }

    T &&operator[](std::size_t index) && {
        return std::move(m_data[index]);
    }

    T &at(size_t index) & {
        check_index_is_in_range(index);
        return m_data[index];
    }

    const T &at(size_t index) const & {
        check_index_is_in_range(index);
        return m_data[index];
    }

    T &&at(size_t index) && {
        check_index_is_in_range(index);
        return std::move(m_data[index]);
    }

    T *begin() {
        return m_data;
    }

    const T *begin() const {
        return m_data;
    }

    T *end() {
        return m_data + m_size;
    }

    const T *end() const {
        return m_data + m_size;
    }

    void reserve(size_t new_capacity) {
        increase_capacity(new_capacity);
    }
};
}  // namespace lab_vector_naive

#endif  // VECTOR_HPP_