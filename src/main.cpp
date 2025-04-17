#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <array>
#include <random>
#include <chrono>

class TicTacToe {
private:
    std::array<std::array<char, 3>, 3> board;
    std::mutex board_mutex;
    std::condition_variable turn_cv;
    char current_player;
    bool game_over;
    char winner;

public:
    TicTacToe() : current_player('X'), game_over(false), winner(' ') {
        for (auto& row : board) {
            row.fill(' ');
        }
    }

    void display_board() {
        std::lock_guard<std::mutex> lock(board_mutex);
        std::cout << "\n";
        for (int i = 0; i < 3; ++i) {
            std::cout << " ";
            for (int j = 0; j < 3; ++j) {
                std::cout << board[i][j];
                if (j < 2) std::cout << " | ";
            }
            if (i < 2) std::cout << "\n-----------\n";
        }
        std::cout << "\n";
    }

    bool make_move(char player, int row, int col) {
        std::unique_lock<std::mutex> lock(board_mutex);
        if (game_over || player != current_player || board[row][col] != ' ') {
            return false;
        }

        board[row][col] = player;

        if (check_win(player)) {
            game_over = true;
            winner = player;
        } else if (check_draw()) {
            game_over = true;
            winner = 'D';
        } else {
            current_player = (current_player == 'X') ? 'O' : 'X';
        }

        turn_cv.notify_all();
        return true;
    }

    bool check_win(char player) {
        for (int i = 0; i < 3; ++i) {
            if ((board[i][0] == player && board[i][1] == player && board[i][2] == player) ||
                (board[0][i] == player && board[1][i] == player && board[2][i] == player)) {
                return true;
            }
        }
        if ((board[0][0] == player && board[1][1] == player && board[2][2] == player) ||
            (board[0][2] == player && board[1][1] == player && board[2][0] == player)) {
            return true;
        }
        return false;
    }

    bool check_draw() {
        for (auto& row : board) {
            for (auto cell : row) {
                if (cell == ' ') return false;
            }
        }
        return true;
    }

    bool is_game_over() {
        std::lock_guard<std::mutex> lock(board_mutex);
        return game_over;
    }

    char get_winner() {
        std::lock_guard<std::mutex> lock(board_mutex);
        return winner;
    }

    void wait_turn(char player) {
        std::unique_lock<std::mutex> lock(board_mutex);
        turn_cv.wait(lock, [&]() {
            return game_over || current_player == player;
        });
    }
};

// Classe Player
class Player {
private:
    TicTacToe& game;
    char symbol;
    std::mt19937 rng;

public:
    Player(TicTacToe& g, char s) : game(g), symbol(s), rng(std::random_device{}()) {}

    void play() {
        while (!game.is_game_over()) {
            game.wait_turn(symbol);
            if (game.is_game_over()) break;

            // Estratégia aleatória
            std::uniform_int_distribution<int> dist(0, 2);
            bool moved = false;

            while (!moved && !game.is_game_over()) {
                int row = dist(rng);
                int col = dist(rng);
                moved = game.make_move(symbol, row, col);
                if (moved) {
                    std::cout << "Jogador " << symbol << " jogou em (" << row << ", " << col << ")\n";
                    game.display_board();
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(300)); // para visualização
            }
        }
    }
};

// Função principal
int main() {
    TicTacToe game;

    Player playerX(game, 'X');
    Player playerO(game, 'O');

    std::thread threadX(&Player::play, &playerX);
    std::thread threadO(&Player::play, &playerO);

    threadX.join();
    threadO.join();

    char winner = game.get_winner();
    if (winner == 'D') {
        std::cout << "\nEmpate!\n";
    } else {
        std::cout << "\nJogador " << winner << " venceu!\n";
    }

    return 0;
}
