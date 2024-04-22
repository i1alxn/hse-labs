
#ifdef _MSC_VER
#include <crtdbg.h>
#endif

#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

const std::string sherlock_holmes_text =
    "I had had so many reasons to believe in my friend's subtle powers of\n"
    "reasoning and extraordinary energy in action that I felt that he must\n"
    "have some solid grounds for the assured and easy demeanour with which\n"
    "he treated the singular mystery which he had been called upon to\n"
    "fathom. Once only had I known him to fail, in the case of the King of\n"
    "Bohemia and of the Irene Adler photograph; but when I looked back to\n"
    "the weird business of the Sign of Four, and the extraordinary\n"
    "circumstances connected with the Study in Scarlet, I felt that it would\n"
    "be a strange tangle indeed which he could not unravel.";

const std::string cat_jeoffry_text = R"(For he is of the tribe of Tiger.
For the Cherub Cat is a term of the Angel Tiger.
For he has the subtlety and hissing of a serpent, which in goodness he suppresses.
For he will not do destruction, if he is well-fed, neither will he spit without provocation.
For he purrs in thankfulness, when God tells him he's a good Cat.
For he is an instrument for the children to learn benevolence upon.
For every house is incomplete without him and a blessing is lacking in the spirit.
For the Lord commanded Moses concerning the cats at the departure of the Children of Israel from Egypt.
For every family had one cat at least in the bag.
For the English Cats are the best in Europe.)";

enum class Direction { Up, Down, Left, Right };
std::map<Direction, std::pair<int, int>>
    direction_map =  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    {{Direction::Up, {0, 1}},
     {Direction::Down, {0, -1}},
     {Direction::Left, {-1, 0}},
     {Direction::Right, {1, 0}}};
std::pair<int, int>
    position =  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    {0, 0};
std::set<std::pair<int, int>>
    map =  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    {position};
Direction
    current_direction =  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    Direction::Up;

void print_quote(const std::string &quote, std::ostream &out = std::cout) {
    out << quote << "\n";
}

void print_sherlock() {
    print_quote(sherlock_holmes_text);
}

void print_cat() {
    print_quote(cat_jeoffry_text);
}

void move_forward() {
    position.first += direction_map[current_direction].first;
    position.second += direction_map[current_direction].second;
    map.insert(position);
}

void move_backward() {
    position.first -= direction_map[current_direction].first;
    position.second -= direction_map[current_direction].second;
    map.insert(position);
}

void turn_left() {
    switch (current_direction) {
        case Direction::Up:
            current_direction = Direction::Left;
            break;
        case Direction::Down:
            current_direction = Direction::Right;
            break;
        case Direction::Left:
            current_direction = Direction::Down;
            break;
        case Direction::Right:
            current_direction = Direction::Up;
            break;
    }
}

void turn_right() {
    switch (current_direction) {
        case Direction::Up:
            current_direction = Direction::Right;
            break;
        case Direction::Down:
            current_direction = Direction::Left;
            break;
        case Direction::Left:
            current_direction = Direction::Up;
            break;
        case Direction::Right:
            current_direction = Direction::Down;
            break;
    }
}

void show_map() {
    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;
    for (const auto &point : map) {
        minX = std::min(minX, point.first);
        maxX = std::max(maxX, point.first);
        minY = std::min(minY, point.second);
        maxY = std::max(maxY, point.second);
    }
    for (int i = 0; i <= 2 * (maxX - minX) + 2; i++) {
        std::cout << ' ';
    }
    std::cout << std::endl;
    for (int y = maxY; y >= minY; y--) {
        for (int x = minX; x <= maxX; x++) {
            if (x == position.first && y == position.second) {
                if (current_direction == Direction::Up) {
                    std::cout << ' ' << '^';
                } else if (current_direction == Direction::Down) {
                    std::cout << ' ' << 'v';
                } else if (current_direction == Direction::Left) {
                    std::cout << ' ' << '<';
                } else if (current_direction == Direction::Right) {
                    std::cout << ' ' << '>';
                }
            } else if (map.find({x, y}) != map.end()) {
                std::cout << ' ' << "o";
            } else {
                std::cout << ' ' << ".";
            }
        }
        std::cout << ' ' << "\n";
        for (int i = 0; i <= 2 * (maxX - minX) + 2; i++) {
            std::cout << ' ';
        }
        std::cout << "\n";
    }
}

int main() {  // NOLINT(readability-function-cognitive-complexity)
#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE | _CRTDBG_MODE_DEBUG);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);
#endif
    std::string command;
    std::deque<std::string> macros;

    while (std::cin >> command) {
        if (command == "quit" || command == "exit") {
            break;
        } else if (command == "start-macro") {
            macros.clear();
            while (std::cin >> command && command != "end-macro") {
                if (command == "fwd" || command == "forward" ||
                    command == "bwd" || command == "backward" ||
                    command == "turn-right" || command == "right" ||
                    command == "turn-left" || command == "left" ||
                    command == "show-map") {
                    macros.push_back(command);
                    std::cout << "Recorded"
                              << "\n";
                }
            }
            std::cout << "New macro of " << static_cast<int>(macros.size())
                      << " command(s)"
                      << "\n";
        } else if (command == "run-macro") {
            auto iter = macros.begin();
            while (iter != macros.end()) {
                if (*iter == "fwd" || *iter == "forward") {
                    move_forward();
                } else if (*iter == "bwd" || *iter == "backward") {
                    move_backward();
                } else if (*iter == "right" || *iter == "turn-right") {
                    turn_right();
                } else if (*iter == "left" || *iter == "turn-left") {
                    turn_left();
                } else if (*iter == "show-map") {
                    show_map();
                }
                ++iter;
            }
        } else if (command == "sherlock-holmes" || command == "sherlock") {
            print_sherlock();
        } else if (command == "cat-jeoffry" || command == "cat") {
            print_cat();
        } else if (command == "fwd" || command == "forward") {
            move_forward();
        } else if (command == "bwd" || command == "backward") {
            move_backward();
        } else if (command == "turn-right" || command == "right") {
            turn_right();
        } else if (command == "turn-left" || command == "left") {
            turn_left();
        } else if (command == "show-map") {
            show_map();
        }
    }
    return 0;
}