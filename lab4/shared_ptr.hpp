#pragma once

#include <algorithm>
#include <iostream>
#include <utility>

namespace ptrs::shared {
template <typename T>
class shared_ptr {
    T *ptr = nullptr;
    int *counter = nullptr;

    void increase_counter() {
        if (counter != nullptr) {
            ++*counter;
        }
    }

    void reset_(T *new_ptr, int *new_counter) {
        if (counter != nullptr && --*counter == 0) {
            delete ptr;
            delete counter;
        }
        ptr = new_ptr;
        counter = new_counter;
    }

public:
    shared_ptr() = default;

    // cppcheck-suppress noExplicitConstructor
    shared_ptr(std::nullptr_t) {
    }

    explicit shared_ptr(T *ptr) : ptr(ptr) {
        if (ptr != nullptr) {
            counter = new int(1);
        }
    }

    shared_ptr(const shared_ptr &other)
        // cppcheck-suppress copyCtorPointerCopying
        : ptr(other.ptr), counter(other.counter) {
        increase_counter();
    }

    shared_ptr(shared_ptr &&other)
        : ptr(std::exchange(other.ptr, nullptr)),
          counter(std::exchange(other.counter, nullptr)) {
    }

    shared_ptr &operator=(const shared_ptr &other) {
        if (this != &other) {
            reset_(other.ptr, other.counter);
            increase_counter();
        }
        return *this;
    }

    shared_ptr &operator=(shared_ptr &&other) {
        if (this != &other) {
            reset_(
                std::exchange(other.ptr, nullptr),
                std::exchange(other.counter, nullptr)
            );
        }
        return *this;
    }

    ~shared_ptr() {
        reset_(nullptr, nullptr);
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

    void reset(T *new_ptr = nullptr) {
        reset_(new_ptr, new_ptr == nullptr ? nullptr : new int(1));
    }

    explicit operator bool() const {
        return ptr != nullptr;
    }

    friend void swap(shared_ptr &lhs, shared_ptr &rhs) {
        std::swap(lhs.ptr, rhs.ptr);
        std::swap(lhs.counter, rhs.counter);
    }

    friend bool operator==(const shared_ptr &lhs, const shared_ptr &rhs) {
        return lhs.ptr == rhs.ptr;
    }

    friend bool operator!=(const shared_ptr &lhs, const shared_ptr &rhs) {
        return !(lhs == rhs);
    }
};
}  // namespace ptrs::shared