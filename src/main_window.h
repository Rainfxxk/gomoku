#include <stdio.h>
#include <unistd.h>
#include "tui.h"
#include "chess.h"
#include "chessboard.h"
#include "timer.h"

class main_window : public tui_widght {

public:
    enum game_state {
        STATE_START,
        STATE_TWO_PLAYER,
        STATE_AI_PLAYER,
        STATE_GAME_OVER,
        STATE_EXIT
    };

public:
    game_state current_state;
    tui_stack_widget* stack;
    tui_selector* selector;
    tui_layout* layout;
    chessboard_widght* chessboard;
    tui_widght* right_panel;
    tui_layout* right_layout;
    tui_widght* status_panel;
    tui_layout* status_layout;
    tui_widght* help_panel;
    tui_layout* help_layout;
    tui_label* status_label;
    tui_label* move_label;
    tui_label* put_label;
    tui_label* quit_label;
    int time;
    tui_label* time_label;
    timer game_timer;

public:
    main_window(int x, int y, int width, int height, bool is_bordered, std::string name);
    ~main_window();
    void timer_start();
    void timer_stop();
    char get_keyboard_input();
    void start(char c);
    std::pair<int, int> ai();
    bool is_game_over(int row, int col);
    void put_chess_piece(bool is_ai = false);
    void two_player(char c);
    void ai_game(char c);
    void run();
};