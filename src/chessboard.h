#include "tui.h"
#include "chess.h"

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