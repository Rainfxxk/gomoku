#include "chessboard.h"

chessboard_widght::chessboard_widght(int row, int col, std::string name)
    : tui_widght(col + 2, row + 2, true, name) {
    game = new chess(row, col);
}

chessboard_widght::~chessboard_widght() {
    delete game;
}

void chessboard_widght::draw_chessboard_ref(int x, int y) {
    // Draw coordinates if needed
    for (int i = 0; i < game->col; ++i) {
        move_cursor(x + 1 + i * 2, y - 1);
        printf("%2c", 'a' + i); // Top border
    }
    for (int i = 0; i < game->row; ++i) {
        move_cursor(x - 2, y + 1 + i);
        printf("%2d", i + 1); // Left border
    }
}

void chessboard_widght::draw_chessboard_border(int x, int y) {
    const char *border_string[] = {"┏", "━┯", "━┓", "─┨", "━┛", "━┷", "┗", "┠"};
    int width = game->col;
    int height = game->row;

    move_cursor(x, y);
    printf("%s", border_string[0]); // Top-left corner

    for (int i = 0; i < width; ++i) {
        printf("%s", border_string[1]); // Top border
    }

    printf("%s", border_string[2]); // Top-right corner

    for (int j = 0; j < height; ++j) {
        move_cursor(x, y + j + 1);
        printf("%s", border_string[7]); // Right border
        move_cursor(x + width * 2 + 1, y + j + 1);
        printf("%s", border_string[3]); // Left border
    }

    move_cursor(x, y + height + 1);
    printf("%s", border_string[6]); // Bottom-left corner

    for (int i = 0; i < width; ++i) {
        printf("%s", border_string[5]); // Bottom border
    }

    printf("%s", border_string[4]); // Bottom-right corner
    fflush(stdout);
}

void chessboard_widght::move_preview_y(int direction) {
    int count = 0;
    int offset = (direction > 0) ? 1 : -1;
    preview_y = (preview_y + offset + game->row) % game->row;
}

void chessboard_widght::move_preview_x(int direction) {
    int count = 0;
    int offset = (direction > 0) ? 1 : -1;
    preview_x = (preview_x + offset + game->col) % game->col;
}

void chessboard_widght::draw_chessboard() {
    int x = this->pos_x + (width - game->col * 2 - 2) / 2;
    int y = this->pos_y + (height - game->row - 2) / 2;

    draw_chessboard_ref(x, y);
    draw_chessboard_border(x, y);

    for (int i = 0; i < game->row; ++i) {
        move_cursor(x + 1, y + 1 + i);
        for (int j = 0; j < game->col; ++j) {
            int previewed = 0;
            if (preview_x == j && preview_y == i && is_preview) {
                previewed = 1;
            }
            chess::chess_piece piece = (*game)[i][j];
            switch (piece) {
                case chess::EMPTY:
                    printf( previewed ? "─×" : "─┼");
                    break;
                case chess::BLACK:
                    printf( previewed ? "─󰅙" : "─󰧞");
                    break;
                case chess::WHITE:
                    printf( previewed ? "─󰅚" : "─󰄰");
                    break;
            }
        }
    }
    fflush(stdout);
}

void chessboard_widght::draw() {
    // Draw the chess board
    tui_widght::draw(); // Draw base widget (if any)
    draw_chessboard();

    fflush(stdout);
}