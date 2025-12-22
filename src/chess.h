#pragma once
#include <cstddef>
#include <vector>
#include "tui.h"

class chess {
public:
    enum chess_piece {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };

public:
    chess_piece **board;
    int count;

public:
    int row;
    int col;

private:
    int calcute_score(int r, int c, int delta_row, int delta_col);
    int evaluate_board();
    std::pair<int, std::pair<int, int>> minimax(int depth, bool is_maximizing, int alpha, int beta);

public:
    chess(int row, int col);
    ~chess();

    bool is_ocupied(int row, int col);
    bool put_chess_piece(int row, int col, chess_piece piece);
    int count_piece(int row, int col, int delta_row, int delta_col);
    int count_alive_n(int r, int c, int delta_row, int delta_col, int n);
    bool check_alive_three(int row, int col);
    bool check_alive_four(int row, int col);
    int check_ban(int row, int col);
    bool check_win(int row, int col);
    bool check_full();
    void clear();
    std::pair<int, int> get_ai_move(int depth = 3);  // 获取 AI 的下一步棋
    chess_piece *operator[](int index);
};
