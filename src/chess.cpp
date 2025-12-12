#include "chess.h"

chess::chess(int row, int col) {
    this->row = row;
    this->col = col;
    count = 0;
    this->board = new chess_piece*[row];
    for (int i = 0; i < row; ++i) {
        this->board[i] = new chess_piece[col];
        for (int j = 0; j < col; ++j) {
            this->board[i][j] = EMPTY;
        }
    }
}

chess::~chess() {
    // Assuming row size is known; in real code, store row size as a member variable
    for (int i = 0; i < this->row; ++i) {
        delete[] this->board[i];
    }
    delete[] this->board;
}

bool chess::is_ocupied(int row, int col) {
    return this->board[row][col] != EMPTY;
}

bool chess::put_chess_piece(int row, int col, chess_piece piece) {
    if (is_ocupied(row, col)) {
        return false;
    }
    this->board[row][col] = piece;
    count++;
    return true;
}

void chess::clear() {
    for (int i = 0; i < this->row; ++i) {
        for (int j = 0; j < this->col; ++j) {
            this->board[i][j] = EMPTY;
        }
    }
}

chess::chess_piece* chess::operator [](int index) {
    return this->board[index];
}

int chess::count_piece(int r, int c, int delta_row, int delta_col) {
    chess_piece piece = board[r][c];
    if (piece == EMPTY) return 0;

    int count = 1;

    // 向正方向计数
    int nr = r + delta_row;
    int nc = c + delta_col;
    while (nr >= 0 && nr < row && nc >= 0 && nc < col && board[nr][nc] == piece) {
        count++;
        nr += delta_row;
        nc += delta_col;
    }

    // 向反方向计数
    nr = r - delta_row;
    nc = c - delta_col;
    while (nr >= 0 && nr < row && nc >= 0 && nc < col && board[nr][nc] == piece) {
        count++;
        nr -= delta_row;
        nc -= delta_col;
    }

    return count;
}

bool chess::check_alive_three(int r, int c) {
    // Implementation to check for alive three
    int count = 0;
    if (count_piece(r, c, 0, 1) == 3) count++; // Horizontal
    if (count_piece(r, c, 1, 0) == 3) count++; // Vertical
    if (count_piece(r, c, 1, 1) == 3) count++; // Diagonal 
    if (count_piece(r, c, 1, -1) == 3) count++; // Diagonal /  
    return count > 1; // Placeholder
}

bool chess::check_alive_four(int r, int c) {
    // Implementation to check for alive four
    int count = 0;
    if (count_piece(r, c, 0, 1) == 4) count++; // Horizontal
    if (count_piece(r, c, 1, 0) == 4) count++; // Vertical
    if (count_piece(r, c, 1, 1) == 4) count++; // Diagonal 
    if (count_piece(r, c, 1, -1) == 4) count++; // Diagonal /  
    return count > 1; // Placeholder
}


int chess::check_ban(int r, int c) {
    if (check_alive_three(r, c) || check_alive_four(r, c)) {
        return true;
    }
    return false;
}

bool chess::check_win(int r, int c) {
    // Implementation to check for long line
    if (count_piece(r, c, 0, 1) > 4) return true; // Horizontal
    if (count_piece(r, c, 1, 0) > 4) return true; // Vertical
    if (count_piece(r, c, 1, 1) > 4) return true; // Diagonal 
    if (count_piece(r, c, 1, -1) > 4) return true; // Diagonal /  
    return false;
}

bool chess::check_full() {
    return count >= row * col;
}

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