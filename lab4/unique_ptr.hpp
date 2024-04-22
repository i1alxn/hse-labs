#pragma once

#include <algorithm>
#include <iostream>
#include <utility>

namespace ptrs::unique {
template <typename T>
struct Deleter {
    void operator()(T *ptr) const {
        delete ptr;
    }
};

template <typename T, typename Deleter = Deleter<T>>
class unique_ptr {
    T *ptr = nullptr;
    Deleter deleter;

public:
    unique_ptr() = default;

    // cppcheck-suppress noExplicitConstructor
    unique_ptr(std::nullptr_t) {
    }

    explicit unique_ptr(T *ptr) : ptr(ptr) {
    }

    explicit unique_ptr(T *ptr, const Deleter &deleter)
        : ptr(ptr), deleter(deleter) {
    }

    explicit unique_ptr(T *ptr, Deleter &&deleter)
        : ptr(ptr), deleter(std::move(deleter)) {
    }

    unique_ptr(const unique_ptr &) = delete;

    unique_ptr(unique_ptr &&other)
        : ptr(std::exchange(other.ptr, nullptr)),
          deleter(std::move(other.deleter)) {
    }

    unique_ptr &operator=(const unique_ptr &) = delete;

    unique_ptr &operator=(unique_ptr &&other) {
        if (this != &other) {
            reset(other.release());
            deleter = std::move(other.deleter);
        }
        return *this;
    }

    ~unique_ptr() {
        reset();
    }

    [[nodiscard]] T *get() const {
        return ptr;
    }

    T &operator*() const {
        return *ptr;
    }

    T *operator->() const {
        return ptr;
    }

    explicit operator bool() const {
        return ptr != nullptr;
    }

    T *release() {
        return std::exchange(ptr, nullptr);
    }

    void reset(T *new_ptr = nullptr) {
        if (ptr != nullptr) {
            deleter(ptr);
        }
        ptr = new_ptr;
    }

    friend bool operator==(const unique_ptr &lhs, const unique_ptr &rhs) {
        return lhs.ptr == rhs.ptr;
    }

    friend bool operator!=(const unique_ptr &lhs, const unique_ptr &rhs) {
        return !(lhs == rhs);
    }

    friend void swap(unique_ptr &lhs, unique_ptr &rhs) {
        std::swap(lhs.ptr, rhs.ptr);
        std::swap(lhs.deleter, rhs.deleter);
    }
};

}  // namespace ptrs::unique
