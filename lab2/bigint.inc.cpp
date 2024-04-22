#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

const int BASE = 1000;

struct bigint {
private:
    std::vector<unsigned> digits;

public:
    bigint() {
        digits.push_back(0);
    }

    // cppcheck-suppress noExplicitConstructor
    bigint(unsigned number) {
        if (number == 0) {
            digits.push_back(0);
        }
        while (number != 0) {
            digits.push_back(number % BASE);
            number = number / BASE;
        }
    }

    [[maybe_unused]] void delete_leading_zeros_from_bigint() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }

    static unsigned GetDeg() {
        return static_cast<unsigned>(log10(BASE));
    }

    explicit bigint(const std::string &string) {
        std::string temp = string;
        delete_leading_zeros_from_string(temp);
        for (std::size_t i = temp.size(); i > 0; i -= GetDeg()) {
            if (i < GetDeg()) {
                digits.push_back(stoi(temp.substr(0, i)));
                break;
            } else {
                digits.push_back(stoi(temp.substr(i - GetDeg(), GetDeg())));
            }
        }
    }

    explicit operator unsigned int() const {
        unsigned int answer = 0;
        std::size_t number_size = digits.size();
        for (std::size_t counter = number_size; counter > 0; counter--) {
            answer *= BASE;
            answer += digits[counter - 1];
        }
        return answer;
    }

    [[maybe_unused]] static void delete_leading_zeros_from_string(
        std::string &string
    ) {
        while (string[0] == '0' && string.size() > 1) {
            string.erase(string.begin());
        }
    }

    [[nodiscard]] std::string to_string() const {
        std::string basic_string;
        std::size_t number_size = digits.size();
        for (std::size_t i = number_size; i > 0; i--) {
            std::string tmp;
            tmp = std::to_string(digits[i - 1]);
            if (i < number_size && tmp.size() < GetDeg()) {
                while (tmp.size() != GetDeg()) {
                    tmp.insert(tmp.begin(), '0');
                }
            }
            basic_string += tmp;
        }
        return basic_string;
    }

    friend bool operator==(const bigint &lhs, const bigint &rhs);

    friend bool operator!=(const bigint &lhs, const bigint &rhs);

    friend bool operator>(const bigint &lhs, const bigint &rhs);

    friend bool operator>=(const bigint &lhs, const bigint &rhs);

    friend bool operator<(const bigint &lhs, const bigint &rhs);

    friend bool operator<=(const bigint &lhs, const bigint &rhs);

    friend bigint operator+(const bigint &lhs, const bigint &rhs);

    friend bigint operator+=(bigint &lhs, const bigint &rhs);

    friend bigint operator-(const bigint &lhs, const bigint &rhs);

    friend bigint operator-=(bigint &lhs, const bigint &rhs);
};

bool operator==(const bigint &lhs, const bigint &rhs) {
    return lhs.digits == rhs.digits;
}

bool operator!=(const bigint &lhs, const bigint &rhs) {
    return lhs.digits != rhs.digits;
}

bool operator<(const bigint &lhs, const bigint &rhs) {
    if (lhs.digits.size() != rhs.digits.size()) {
        return lhs.digits.size() < rhs.digits.size();
    }
    for (std::size_t i = lhs.digits.size(); i > 0; i--) {
        if (lhs.digits[i - 1] != rhs.digits[i - 1]) {
            return lhs.digits[i - 1] < rhs.digits[i - 1];
        }
    }
    return false;
}

bool operator>(const bigint &lhs, const bigint &rhs) {
    return rhs < lhs;
}

bool operator>=(const bigint &lhs, const bigint &rhs) {
    return !(lhs < rhs);
}

bool operator<=(const bigint &lhs, const bigint &rhs) {
    return !(lhs > rhs);
}

bigint operator+(const bigint &lhs, const bigint &rhs) {
    bigint result;
    std::size_t max_size = std::max(lhs.digits.size(), rhs.digits.size());
    result.digits = std::vector<unsigned>(max_size + 1, 0);

    for (std::size_t i = 0; i < max_size; i++) {
        if (i < lhs.digits.size()) {
            result.digits[i] += lhs.digits[i];
        }
        if (i < rhs.digits.size()) {
            result.digits[i] += rhs.digits[i];
        }
        result.digits[i + 1] += result.digits[i] / BASE;
        result.digits[i] %= BASE;
    }
    result.delete_leading_zeros_from_bigint();
    return result;
}

bigint operator+=(bigint &lhs, const bigint &rhs) {
    lhs = lhs + rhs;
    return lhs;
}

bigint operator-(const bigint &lhs, const bigint &rhs) {
    bigint result;
    result.digits = lhs.digits;

    for (std::size_t i = 0; i < lhs.digits.size(); i++) {
        if (i < rhs.digits.size()) {
            if (result.digits[i] < rhs.digits[i]) {
                result.digits[i] += BASE;
                result.digits[i] -= rhs.digits[i];
                std::size_t j = i + 1;
                while (result.digits[j] == 0) {
                    result.digits[j] += BASE - 1;
                    j++;
                }
                result.digits[j]--;
            } else {
                result.digits[i] -= rhs.digits[i];
            }
        }
    }
    result.delete_leading_zeros_from_bigint();
    return result;
}

bigint operator-=(bigint &lhs, const bigint &rhs) {
    lhs = lhs - rhs;
    return lhs;
}
