#pragma once
#include <cstddef>
#include "tui.h"

class chess
{
public:
    enum chess_piece
    {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
private:
   chess_piece **board; 
    /* data */
public:
    int row;
    int col;
    int count;
public:
    chess(int row, int col);
    ~chess();
    bool is_ocupied(int row, int col);
    bool put_chess_piece(int row, int col, chess_piece piece);
    int count_piece(int row, int col, int delta_row, int delta_col);
    bool check_alive_three(int row, int col);
    bool check_alive_four(int row, int col);
    int check_ban(int row, int col);
    bool check_win(int row, int col);
    bool check_full();
    void clear();
    chess_piece *operator [](int index);
};

class chessboard_widght : public tui_widght {

public:
    chess* game;
    bool is_preview;
    int preview_x;
    int preview_y;

    chess::chess_piece current_turn;

private:
    void draw_chessboard_ref(int x, int y);
    void draw_chessboard_border(int x, int y);
    void draw_chessboard();
    /* data */
public:
    chessboard_widght(int row, int col);
    chessboard_widght(int row, int col, std::string name);
    ~chessboard_widght();
    void move_preview_y(int direcition);
    void move_preview_x(int direcition);
    void draw() override;
};