#include <iostream>
#include <vector>
#include <windows.h>
#include <random>
#include <deque>
#include <conio.h>

using namespace std;

constexpr int BOARD_SIZE = 16;
constexpr int DELAY = 300; // ms

struct Coords {
    int x, y;
    bool operator==(const Coords& other) const {
        return x == other.x && y == other.y;
    }
};

class Game {
    mt19937 gen;
    uniform_int_distribution<> distrib;
    vector<vector<int>> board;
    deque <Coords> tail;
    Coords snake_pos;
    Coords apple_pos;
    int direction; // 0 - вверх, 1 - вправо, 2 - вниз, 3 - влево
    int score = 0;

    void update_apple() {
        do {
            apple_pos = { distrib(gen), distrib(gen) };
        } while (board[apple_pos.x][apple_pos.y] != 0);
    }

    void clear_board() {
        for (auto& row : board) {
            fill(row.begin(), row.end(), 0);
        }
    }

    void display_snake() {
        board[snake_pos.x][snake_pos.y] = 1;
        for (const auto& seg : tail) {
            board[seg.x][seg.y] = 2;
        }
    }

public:
    Game() : gen(random_device{}()), distrib(0, BOARD_SIZE - 1),
        board(BOARD_SIZE, vector<int>(BOARD_SIZE, 0)),
        tail({ {7, 6} }), snake_pos({ 7, 7 }), direction(1), score(0) {
        update_apple();
    }

    void handle_input() {
        if (_kbhit()) {
            const int key = _getch();

            if (key == 0 || key == 224) {
                const int ext_code = _getch();

                switch (ext_code) {
                case 72:
                    if (direction != 2) direction = 0;
                    break;
                case 77:
                    if (direction != 3) direction = 1;
                    break;
                case 80:
                    if (direction != 0) direction = 2;
                    break;
                case 75:
                    if (direction != 1) direction = 3;
                    break;
                }
            }
        }
    }

    void update() {
        const Coords old_pos = snake_pos;

        // Move head
        switch (direction) {
            case 0: snake_pos.x--; break;
            case 1: snake_pos.y++; break;
            case 2: snake_pos.x++; break;
            case 3: snake_pos.y--; break;
        }

        // Check collisions
        if (snake_pos.x < 0 || snake_pos.y < 0 || snake_pos.x >= BOARD_SIZE || snake_pos.y >= BOARD_SIZE) {
            cout << "Game over";
            exit(0);
        }

        // Check if apple was eaten
        if (snake_pos == apple_pos) {
            score++;
            update_apple();
            tail.push_back(old_pos);
        }
        else {
            tail.pop_front();
            tail.push_back(old_pos);
        }

        // Check self-collision
        for (const auto& segment : tail) {
            if (snake_pos == segment) {
                cout << "Game over";
                exit(0);
            }
        }
    }

    void render() {
        clear_board();
        display_snake();
        board[apple_pos.x][apple_pos.y] = 3;

        system("cls");
        for (const auto& row : board) {
            for (int cell : row) {
                if (cell == 0) {
                    cout << ".";
                }
                else if (cell == 1) {
                    cout << "e";
                }
                else if (cell == 2) {
                    cout << "o";
                }
                else if (cell == 3) {
                    cout << "*";
                }
                cout << ' ';
            }
            cout << '\n';
        }

        

        cout << "Score: " << score << "\n";
    }
};

int main()
{
    ios_base::sync_with_stdio(false);
    cout.tie(nullptr);

    Game game;

    while (true) {
        game.handle_input();
        game.update();
        game.render();
        Sleep(DELAY);
    }
}