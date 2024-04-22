#include <algorithm>
#include <fstream>
#include <memory>
#include <sstream>
#include <unordered_map>
#include <utility>
#include "matrix.hpp"
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

namespace matrix_interpreter {
struct interpreter_exception : std::runtime_error {
    explicit interpreter_exception(const std::string &exception_text)
        : std::runtime_error(exception_text) {
    }
};

struct invalid_command_format : interpreter_exception {
    invalid_command_format() : interpreter_exception("Invalid command format") {
    }
};

struct invalid_file_format : interpreter_exception {
    invalid_file_format() : interpreter_exception("Invalid file format") {
    }
};

struct unable_to_open : interpreter_exception {
    explicit unable_to_open(const std::string &file_path)
        : interpreter_exception("Unable to open file \'" + file_path + "\'") {
    }
};

struct not_a_register : interpreter_exception {
    explicit not_a_register(const std::string &wrong_register_format_text)
        : interpreter_exception(
              "\'" + wrong_register_format_text + "\'" + " is not a register"
          ) {
    }
};

struct command {
    virtual ~command() = default;

    command() = default;

    command(const command &) = delete;

    command(command &&) = delete;

    command &operator=(const command &) = delete;

    command &operator=(command &&) = delete;

    virtual void
    execute(const std::vector<std::string> &arguments, std::vector<matrix> &registers, bool &) = 0;

protected:
    static void check_if_number_of_arguments_is_correct(
        const std::vector<std::string> &arguments,
        size_t expected_number_of_arguments
    ) {
        if (arguments.size() != expected_number_of_arguments) {
            throw invalid_command_format();
        }
    }

    static void check_if_register_format_is_correct(
        const std::string &register_name
    ) {
        if (register_name.size() != 2 || register_name[0] != '$' ||
            isdigit(register_name[1]) == 0) {
            throw not_a_register(register_name);
        }
    }

    static void check_if_register_index_is_correct(
        int index,
        const std::vector<matrix> &registers
    ) {
        if (index < 0 || index >= static_cast<int>(registers.size())) {
            throw not_a_register("$" + std::to_string(index));
        }
    }

    static size_t check_register_correctness_and_get_index(
        const std::string &arguments,
        const std::vector<matrix> &registers
    ) {
        check_if_register_format_is_correct(arguments);
        const int index = arguments[1] - '0';
        check_if_register_index_is_correct(index, registers);
        return index;
    }
};

struct print_command : command {
    void
    execute(const std::vector<std::string> &arguments, std::vector<matrix> &registers, bool &)
        override {
        check_if_number_of_arguments_is_correct(arguments, 1);
        const size_t index =
            check_register_correctness_and_get_index(arguments[0], registers);
        const size_t rows = registers[index].get_rows();
        const size_t columns = registers[index].get_columns();
        for (size_t row = 0; row < rows; ++row) {
            for (size_t column = 0; column < columns; ++column) {
                std::cout << registers[index].get(row, column);
                if (column != columns - 1) {
                    std::cout << " ";
                }
            }
            std::cout << std::endl;
        }
    }
};

struct load_command : command {
    void
    execute(const std::vector<std::string> &arguments, std::vector<matrix> &registers, bool &)
        override {
        check_if_number_of_arguments_is_correct(arguments, 2);
        const size_t index =
            check_register_correctness_and_get_index(arguments[0], registers);
        registers[index] = load_from_file(arguments[1]);
    }

private:
    static matrix load_from_file(const std::string &file_path) {
        std::fstream file(file_path);
        if (!file.is_open()) {
            throw unable_to_open(file_path);
        }
        long long rows = 0;
        if (!(file >> rows)) {
            throw invalid_file_format();
        }

        long long columns = 0;
        if (!(file >> columns)) {
            throw invalid_file_format();
        }

        if (rows == 0 || columns == 0) {
            return matrix(std::vector<std::vector<long long>>());
        }

        if (rows < 0 || columns < 0 || rows > 1000000 || columns > 1000000) {
            throw invalid_file_format();
        }
        std::vector<std::vector<long long>> data(
            rows, std::vector<long long>(columns)
        );

        for (int row = 0; row < rows; ++row) {
            for (int column = 0; column < columns; ++column) {
                if (!(file >> data[row][column])) {
                    throw invalid_file_format();
                }
            }
        }

        return matrix(std::move(data));
    }
};

struct get_element_command : command {
public:
    void
    execute(const std::vector<std::string> &arguments, std::vector<matrix> &registers, bool &)
        override {
        check_if_number_of_arguments_is_correct(arguments, 3);
        const size_t index =
            check_register_correctness_and_get_index(arguments[0], registers);
        const int row = get_matrix_parameter_from_token(arguments[1]);
        const int column = get_matrix_parameter_from_token(arguments[2]);
        std::cout << registers[index].get(row, column) << std::endl;
    }

private:
    static int get_matrix_parameter_from_token(const std::string &token) {
        for (const auto &sign : token) {
            if (std::isdigit(sign) == 0) {
                throw invalid_command_format();
            }
        }
        long long first_not_zero = 0;
        for (; first_not_zero < static_cast<long long>(token.size()) - 1 &&
               token[first_not_zero] == '0';
             ++first_not_zero) {
        }
        std::string const argument_without_leading_zeros(
            token.begin() + first_not_zero, token.end()
        );
        if (argument_without_leading_zeros.size() > 7) {
            throw invalid_command_format();
        }
        const int matrix_parameter = std::stoi(argument_without_leading_zeros);
        if (matrix_parameter > 1000000) {
            throw invalid_command_format();
        }
        return matrix_parameter;
    }
};

struct add_command : command {
    void
    execute(const std::vector<std::string> &arguments, std::vector<matrix> &registers, bool &)
        override {
        check_if_number_of_arguments_is_correct(arguments, 2);
        const size_t lhs_index =
            check_register_correctness_and_get_index(arguments[0], registers);
        const size_t rhs_index =
            check_register_correctness_and_get_index(arguments[1], registers);
        registers[lhs_index] += registers[rhs_index];
    }
};

struct mul_command : command {
public:
    void
    execute(const std::vector<std::string> &arguments, std::vector<matrix> &registers, bool &)
        override {
        check_if_number_of_arguments_is_correct(arguments, 2);
        const size_t lhs_index =
            check_register_correctness_and_get_index(arguments[0], registers);
        const size_t rhs_index =
            check_register_correctness_and_get_index(arguments[1], registers);
        registers[lhs_index] *= registers[rhs_index];
    }
};

struct exit_command : command {
public:
    void execute(
        const std::vector<std::string> &arguments,
        std::vector<matrix> &,
        bool &is_running
    ) override {
        check_if_number_of_arguments_is_correct(arguments, 0);
        is_running = false;
    }
};

}  // namespace matrix_interpreter

int main() {  // NOLINT(readability-function-cognitive-complexity)
#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
    std::vector<matrix_interpreter::matrix> registers(
        10, matrix_interpreter::matrix()
    );

    std::unordered_map<
        std::string, std::unique_ptr<matrix_interpreter::command>>
        commands;
    commands["print"] = std::make_unique<matrix_interpreter::print_command>();
    commands["load"] = std::make_unique<matrix_interpreter::load_command>();
    commands["elem"] =
        std::make_unique<matrix_interpreter::get_element_command>();
    commands["add"] = std::make_unique<matrix_interpreter::add_command>();
    commands["mul"] = std::make_unique<matrix_interpreter::mul_command>();
    commands["exit"] = std::make_unique<matrix_interpreter::exit_command>();
    std::string line;
    bool is_running = true;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            std::cout << "Invalid command format" << std::endl;
            continue;
        }
        std::istringstream iss(line);
        std::string token;
        std::vector<std::string> tokens;
        while (iss >> token) {
            tokens.push_back(token);
        }
        const std::vector<matrix_interpreter::matrix> registers_copy =
            registers;
        const std::string command_name = tokens[0];
        auto command = commands.find(tokens[0]);
        if (command != commands.end()) {
            try {
                tokens.erase(tokens.begin());
                command->second->execute(tokens, registers, is_running);
            } catch (const matrix_interpreter::interpreter_exception &exception
            ) {
                registers = registers_copy;
                std::cout << exception.what() << std::endl;
            } catch (const matrix_interpreter::matrix_exception &exception) {
                registers = registers_copy;
                std::cout << exception.what() << std::endl;
            } catch (const std::out_of_range &) {
                registers = registers_copy;
                std::cout << "Requested element is out of bounds" << std::endl;
            } catch (const std::invalid_argument &) {
                registers = registers_copy;
                std::cout << "Invalid command format" << std::endl;
            } catch (const std::bad_alloc &) {
                registers = registers_copy;
                std::cout << "Unable to allocate memory" << std::endl;
            } catch (const std::overflow_error &) {
                registers = registers_copy;
                std::cout << "Overflow" << std::endl;
            } catch (const std::underflow_error &) {
                registers = registers_copy;
                std::cout << "Underflow" << std::endl;
            } catch (const std::domain_error &) {
                registers = registers_copy;
                std::cout << "Domain error" << std::endl;
            } catch (const std::length_error &) {
                registers = registers_copy;
                std::cout << "Length error" << std::endl;
            } catch (const std::logic_error &) {
                registers = registers_copy;
                std::cout << "Logic error" << std::endl;
            } catch (const std::range_error &) {
                registers = registers_copy;
                std::cout << "Range error" << std::endl;
            } catch (const std::bad_cast &) {
                registers = registers_copy;
                std::cout << "Bad cast" << std::endl;
            } catch (const std::bad_typeid &) {
                registers = registers_copy;
                std::cout << "Bad type id" << std::endl;
            } catch (const std::bad_exception &) {
                registers = registers_copy;
                std::cout << "Bad exception" << std::endl;
            } catch (...) {
                registers = registers_copy;
                std::cout << "Unknown exception" << std::endl;
            }
            if (!is_running) {
                return 0;
            }
        } else {
            std::cout << "Unknown command: \'" + command_name + "\'"
                      << std::endl;
        }
    }
    return 0;
}
