#include "main_window.h"

main_window::main_window(int x, int y, int width, int height, bool is_bordered, std::string name) : tui_widght(x, y, width, height, is_bordered, name) {
    current_state = STATE_START;
    stack = new tui_stack_widget(0, 0, width, height, false, "main_stack");
    this->add(stack);
    // start page
    selector = new tui_selector(0, 0, width, height, true, "start_selector");
    selector->add_option("Two Player Game");
    selector->add_option("AI Game");
    selector->add_option("Exit");
    stack->add(selector);

    // game page
    layout = new tui_layout(tui_layout::LAYOUT_HORIZONTAL, "main_layout");
    chessboard = new chessboard_widght(15, 15, "chessboard");
    right_panel = new tui_widght(0, 0, false, "right_panel");
    right_layout = new tui_layout(tui_layout::LAYOUT_VERTICAL, "right_layout");
    stack->add(layout);
    layout->add(chessboard, 70); // 30% width
    layout->add(right_panel, 30); // 70% width
    right_panel->add(right_layout);

    // status
    status_panel = new tui_widght(0, 0, true, "status_panel");
    right_layout->add(status_panel, 40);
    status_layout = new tui_layout(tui_layout::LAYOUT_VERTICAL, "status_layout");
    status_label = new tui_label(0, 0, right_panel->width - 2, 1, "status_label", "Status: ");
    time_label = new tui_label(0, 0, right_panel->width - 2, 1, "time_label", "");
    status_panel->add(status_layout);
    status_layout->add(status_label, 50);
    status_layout->add(time_label, 50);

    // help
    help_panel = new tui_widght(0, 0, true, "help_panel");
    right_layout->add(help_panel, 60);
    help_layout = new tui_layout(tui_layout::LAYOUT_VERTICAL, "help_layout");
    move_label = new tui_label(0, 0, right_panel->width - 2, 1, "move_label", "h/j/k/l to move preview");
    put_label = new tui_label(0, 0, right_panel->width - 2, 1, "put_label", "<Enter> to place piece");
    quit_label = new tui_label(0, 0, right_panel->width - 2, 1, "quit_label", "q to quit to main menu");
    help_panel->add(help_layout);
    help_layout->add(move_label, 33);
    help_layout->add(put_label, 33);
    help_layout->add(quit_label, 33);

    time = 15;
}

main_window::~main_window() {}

void main_window::timer_start() {
    time = 15;
    game_timer.start(std::chrono::seconds(15), std::chrono::seconds(1), [this]() {
        std::string time_str = (time < 10 ? "0" : "") + std::to_string(time);
        time_label->set_text("Remain Time: " + time_str + "s");
        time--;
        if (time < 0) {
            current_state = STATE_GAME_OVER;
            status_label->set_text("Time's up! " + std::string(chessboard->current_turn == chess::BLACK ? "White" : "Black") + " Wins!");
            game_timer.stop();
        }
    });
}

void main_window::timer_stop() {
    game_timer.stop();
}

char main_window::get_keyboard_input() {
    char c = '\0';
    if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) current_state = STATE_EXIT;
    return c;
}

void main_window::start(char c) {
    switch (c) {
        case 'j':
            selector->set_current_index(1); // down
            selector->draw();
            break;
        case 'k':
            // down
            selector->set_current_index(-1); // up
            selector->draw();
            break;
        case '\n':
        case '\r':
            // enter
            switch(selector->get_current_index()) {
                case 0:
                    current_state = STATE_TWO_PLAYER;
                    stack->set_current_index(1); // switch to game layout
                    chessboard->is_preview = true;
                    chessboard->preview_x = chessboard->game->col / 2;
                    chessboard->preview_y = chessboard->game->row / 2;
                    chessboard->current_turn = chess::BLACK;
                    status_label->set_text("Black's Turn");
                    timer_start();
                    break;
                case 1:
                    current_state = STATE_AI_PLAYER;
                    stack->set_current_index(1); // switch to game layout
                    chessboard->is_preview = true;
                    chessboard->preview_x = chessboard->game->col / 2;
                    chessboard->preview_y = chessboard->game->row / 2;
                    chessboard->current_turn = chess::BLACK;
                    status_label->set_text("Black's Turn");
                    timer_start();
                    break;
                case 2:
                    current_state = STATE_EXIT;
                    break;
                default:
                    break;
            }
            this->clear_screen();
            break;
        default:
            break;
    }
}

std::pair<int, int> main_window::ai() {
    std::pair<int, int> move = chessboard->game->get_ai_move();
    chessboard->game->put_chess_piece(move.first, move.second, chess::WHITE);
    return move;
}

bool main_window::is_game_over(int row, int col) {
    if (chessboard->current_turn == chess::BLACK && chessboard->game->check_ban(row, col)) {
        current_state = STATE_GAME_OVER;
        status_label->set_text("White Wins! Black made a banned move.");
        return true;
    }
    if (chessboard->game->check_win(row, col)) {
        current_state = STATE_GAME_OVER;
        status_label->set_text("" + std::string( chessboard->current_turn == chess::BLACK ? "Black" : "White") + " Wins!");
        return true;
    }
    if (chessboard->game->check_full()) {
        current_state = STATE_GAME_OVER;
        status_label->set_text("Draw! The board is full.");
        return true;
    }

    return false;
}

void main_window::put_chess_piece(bool is_ai) {
    if (chessboard->game->is_ocupied(chessboard->preview_y, chessboard->preview_x) == false) {
        chessboard->game->put_chess_piece(chessboard->preview_y, chessboard->preview_x, chessboard->current_turn);
        timer_stop();
        if (is_game_over(chessboard->preview_y, chessboard->preview_x)) return;
        if (is_ai) {
            status_label->set_text("AI's Turn");
            chessboard->draw();
            status_label->draw();
            chessboard->current_turn = (chessboard->current_turn == chess::BLACK) ? chess::WHITE : chess::BLACK;
            std::pair<int, int> move = ai();
            if (is_game_over(move.first, move.second)) return;
        }
        chessboard->current_turn = (chessboard->current_turn == chess::BLACK) ? chess::WHITE : chess::BLACK;
        status_label->set_text( chessboard->current_turn == chess::BLACK ? "Black's Turn" : "White's Turn");
        timer_start();
    }
}

void main_window::two_player(char c) {
    // Two player game logic
    switch (c) {
        case 'j':
            // Move preview down
            chessboard->move_preview_y(1);
            break;
        case 'k':
            // Move preview up
            chessboard->move_preview_y(-1);
            break;
        case 'h':
            // Move preview left
            chessboard->move_preview_x(-1);
            break;
        case 'l':
            // Move preview right
            chessboard->move_preview_x(1);
            break;
        case '\n':
        case '\r':
            // Place piece
            put_chess_piece(false);
            break;
        case 'q':
            current_state = STATE_START;
            timer_stop();
            clear_screen();
            stack->set_current_index(0); // switch to start layout
            chessboard->game->clear();
            break;
        default:
            break;
    }
}

void main_window::ai_game(char c) {
    // AI game logic
    switch (c) {
        case 'j':
            // Move preview down
            chessboard->move_preview_y(1);
            break;
        case 'k':
            // Move preview up
            chessboard->move_preview_y(-1);
            break;
        case 'h':
            // Move preview left
            chessboard->move_preview_x(-1);
            break;
        case 'l':
            // Move preview right
            chessboard->move_preview_x(1);
            break;
        case '\n':
        case '\r':
            // Place piece
            put_chess_piece(true);
            break;
        case 'q':
            current_state = STATE_START;
            timer_stop();
            clear_screen();
            stack->set_current_index(0); // switch to start layout
            chessboard->game->clear();
            break;
        default:
            break;
    }
}

void main_window::run() {
    this->draw();

    while (current_state != STATE_EXIT) {
        char c = get_keyboard_input();
        switch (current_state) {
            case STATE_START:
                start(c);
                break;
            case STATE_TWO_PLAYER:
                two_player(c);
                break;
            case STATE_AI_PLAYER:
                ai_game(c);
                break;
            case STATE_GAME_OVER:
                if (c == 'q') {
                    current_state = STATE_START;
                    clear_screen();
                    stack->set_current_index(0); // switch to start layout
                    chessboard->game->clear();
                }
            default:
                break;
        }
        this->draw();
    }
};