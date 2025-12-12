#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include "tui.h"
#include <termios.h>
#include <unistd.h>

// 保存终端原始设置
struct termios original_settings;

// 设置终端为不回显模式
void disable_echo() {
    tcgetattr(STDIN_FILENO, &original_settings);
    struct termios raw = original_settings;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

// 恢复终端原始设置
void restore_echo() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_settings);
}

void restore_terminal() {
    // Restore terminal settings to original state
    restore_echo();
    tui_widght::hide_cursor(false);
    printf("\033[?1049l");
    fflush(stdout); // 确保输出被刷新
}

void signal_handler(int signum) {
    restore_terminal();
    exit(signum);
}

void tui_init() {
    // Initialize terminal settings for TUI
    atexit(restore_terminal);
    disable_echo();
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGABRT, signal_handler);
    tui_widght::hide_cursor(true);
    printf("\033[?1049h");
    fflush(stdout);
}

// tui_widght

void tui_widght::move_cursor(int x, int y) {
    // Move cursor to (x, y) in the terminal
    printf("\033[%d;%dH", y + 1, x + 1); // ANSI escape code to move cursor
    // printf("move_cursor to (%d, %d)\n", x, y);
}

void tui_widght::hide_cursor(bool hide) {
    if (hide) {
        printf("\033[?25l"); // Hide cursor
    } else {
        printf("\033[?25h"); // Show cursor
    }
    fflush(stdout);
}

void tui_widght::clear_screen() {
    printf("\033[H\033[2J\033[3J"); // Clear entire screen
    fflush(stdout);
}

tui_widght::tui_widght() : parent(nullptr), children(nullptr), width(0), height(0), pos_x(0), pos_y(0), is_bordered(false) {}

tui_widght::tui_widght(int width, int height, bool is_bordered)
    : parent(nullptr), children(nullptr), next(nullptr), width(width), height(height), pos_x(0), pos_y(0), is_bordered(is_bordered) {}

tui_widght::tui_widght(int width, int height, bool is_bordered, std::string name)
    : parent(nullptr), children(nullptr), next(nullptr), width(width), height(height), pos_x(0), pos_y(0), is_bordered(is_bordered), name(name) {}

tui_widght::tui_widght(int pos_x, int pos_y, int width, int height, bool is_bordered)
    : parent(nullptr), children(nullptr), next(nullptr), width(width), height(height), pos_x(pos_x), pos_y(pos_y), is_bordered(is_bordered) {};

tui_widght::tui_widght(int pos_x, int pos_y, int width, int height, bool is_bordered, std::string name)
    : parent(nullptr), children(nullptr), next(nullptr), width(width), height(height), pos_x(pos_x), pos_y(pos_y), is_bordered(is_bordered), name(name) {};

tui_widght::~tui_widght() {
    // Cleanup children if necessary
    printf("Destroying widget: %s\n", name.c_str());
    if (this->next) {
        delete this->next;
        this->next = nullptr;
    }
    if (this->children) {
        delete this->children;
        this->children = nullptr;
    }
}

void tui_widght::draw_border(const char** border_string, int x, int y, int width, int height) {
    // Draw border at specified position with given dimensions
    move_cursor(x, y);
    printf("%s", border_string[0]); // Top-left corner

    for (int i = 1; i < width - 1; ++i) {
        printf("%s", border_string[1]); // Top border
    }

    printf("%s", border_string[2]); // Top-right corner

    for (int j = 1; j < height - 1; ++j) {
        move_cursor(x, y + j);
        printf("%s", border_string[7]); // Right border
        move_cursor(x + width - 1, y + j);
        printf("%s", border_string[3]); // Left border
    }

    move_cursor(x, y + height - 1);
    printf("%s", border_string[6]); // Bottom-left corner

    for (int i = 1; i < width - 1; ++i) {
        printf("%s", border_string[5]); // Bottom border
    }

    printf("%s", border_string[4]); // Bottom-right corner
    fflush(stdout);
}

void tui_widght::draw_border(const char** border_string) {
    // Draw border using the provided border string
    // border_string format: "╭─╮│╯─╰│"
    if (border_string == nullptr) {
        static const char *default_border[] = {"╭", "─", "╮", "│", "╯", "─", "╰", "│"};
        border_string = default_border;
    }
    draw_border(border_string, pos_x, pos_y, width, height);
}

void tui_widght::draw() {
    // move cursor to widget position
    move_cursor(pos_x, pos_y);
    // printf("Drawing widget at (%d, %d) with size (%d, %d)\n", pos_x, pos_y, width, height);

    // Default draw implementation
    if (is_bordered) {
        draw_border();
        // Draw border logic
    }

    if (children) {
        children->draw();
    }

    if (next) {
        next->draw();
    }
}

void tui_widght::add(tui_widght* child) {
    // Add child widget to this widget
    child->pos_x = is_bordered ? this->pos_x + 1 : this->pos_x;
    child->pos_y = is_bordered ? this->pos_y + 1 : this->pos_y;
    child->width = is_bordered ? this->width - 2 : this->width;
    child->height = is_bordered ? this->height - 2 : this->height;
    this->children = child;
}

void tui_widght::print_structure(int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("  ");
    }
    printf("Widget: %s, Pos: (%d, %d), Size: (%d, %d), Bordered: %s\n",
           name.c_str(), pos_x, pos_y, width, height, is_bordered ? "Yes" : "No");
    if (children) {
        children->print_structure(indent + 1);
    }
    if (next) {
        next->print_structure(indent);
    }
}

// tui_layout
tui_layout::tui_layout(tui_layout_type layout_type = LAYOUT_HORIZONTAL)
    : tui_widght(), layout_type(layout_type) {}

tui_layout::tui_layout(tui_layout_type layout_type, std::string name)
    : tui_widght(0, 0, false, name), layout_type(layout_type) {}

tui_layout::~tui_layout() {
    tui_widght::~tui_widght();
}

void tui_layout::add(tui_widght* child) {
    if (children == nullptr) {
        children = child;
        child->parent = this;
        child->pos_x = this->pos_x;
        child->pos_y = this->pos_y;
        if (layout_type == LAYOUT_HORIZONTAL) {
            child->height = this->height;
        } else if (layout_type == LAYOUT_VERTICAL) {
            child->width = this->width;
        }
        return;
    }
    else {
        tui_widght* current = children;
        int pos;
        int size;
        while (current) {
            if (layout_type == LAYOUT_HORIZONTAL) {
                pos = current->pos_x + current->width;
                size = current->height;
            } else if (layout_type == LAYOUT_VERTICAL) {
                pos = current->pos_y + current->height;
                size = current->width;
            }

            if (!current->next) {
                current->next = child;
                break;
            }
            current = current->next;
        }
        
        child->parent = this;
        if (layout_type == LAYOUT_HORIZONTAL) {
            child->pos_x = pos;
            child->pos_y = this->pos_y;
            child->height = this->height;
            child->width = size;
        } else if (layout_type == LAYOUT_VERTICAL) {
            child->pos_y = pos;
            child->pos_x = this->pos_x;
            child->width = this->width;
            child->height = size;
        }
        
    }
    
}

void tui_layout::add(tui_widght* child, int radio) {
    // Add child widget to this layout
    int cur_x = pos_x;
    int cur_y = pos_y;

    tui_widght* children = this->children;
    int radio_sum = 0;
    while (children) {
        cur_x += children->width;
        cur_y += children->height;
        children = children->next;
        radio_sum += radio;
        assert(radio_sum <= 100);
    }
    if (layout_type == LAYOUT_HORIZONTAL) {
        // Adjust child width based on radio
        child->width = (this->width * radio) / 100;
        child->height = this->height;
        child->pos_x = cur_x;
        child->pos_y = this->pos_y;
    } else if (layout_type == LAYOUT_VERTICAL) {
        // Adjust child height based on radio
        child->height = (this->height * radio) / 100;
        child->width = this->width;
        child->pos_x = this->pos_x;
        child->pos_y = cur_y;
    }

    child->parent = this;
    if (!this->children) {
        this->children = child;
    } else {    
        tui_widght* current = this->children;
        while (current->next) {
            current = current->next;
        }
        current->next = child;
    }
}

void tui_layout::draw() {
    // Draw vertical layout by arranging children vertically
    tui_widght* child = children;
    while (child) {
        child->draw();
        child = child->next;
    }
}

// tui_stack_widget
tui_stack_widget::tui_stack_widget(int pos_x, int pos_y, int width, int height, bool is_bordered, std::string name)
    : tui_widght(pos_x, pos_y, width, height, is_bordered, name), current_index(-1) {}

tui_stack_widget::~tui_stack_widget() {
    for (auto widget : widgets) {
        delete widget;
    }
}

void tui_stack_widget::add(tui_widght* widget) {
    if (!widget) return;
    widgets.push_back(widget);
    widget->pos_x = this->pos_x;
    widget->pos_y = this->pos_y;
    widget->width = this->width;
    widget->height = this->height;
    widget->parent = this;

    // 如果是第一个添加的窗口，直接显示
    if (current_index == -1) {
        current_index = 0;
    }
}

void tui_stack_widget::set_current_index(int index) {
    if (index < 0 || index >= widgets.size()) return;
    current_index = index;
    draw();
}

int tui_stack_widget::get_current_index() const {
    return current_index;
}

tui_widght* tui_stack_widget::get_current_widget() const {
    if (current_index < 0 || current_index >= widgets.size()) return nullptr;
    return widgets[current_index];
}

void tui_stack_widget::draw() {
    if (current_index < 0 || current_index >= widgets.size()) return;

    // 绘制当前选中的子窗口
    widgets[current_index]->draw();
}

void tui_stack_widget::print_structure(int indent) {
    for (int i = 0; i < indent; ++i) {
        printf("  ");
    }
    printf("Stack Widget: %s, Pos: (%d, %d), Size: (%d, %d), Bordered: %s, Current Index: %d\n",
           name.c_str(), pos_x, pos_y, width, height, is_bordered ? "Yes" : "No", current_index);
    for (size_t i = 0; i < widgets.size(); ++i) {
        for (int j = 0; j < indent + 1; ++j) {
            printf("  ");
        }
        widgets[i]->print_structure(indent + 2);
    }
}

tui_selector::tui_selector(int pos_x, int pos_y, int width, int height, bool is_bordered, std::string name)
    : tui_widght(pos_x, pos_y, width, height, is_bordered, name), current_index(-1) {}

tui_selector::~tui_selector() {
    // 默认析构函数
}

void tui_selector::add_option(const std::string& option) {
    options.push_back(option);
    if (current_index == -1) {
        current_index = 0; // 默认选中第一个选项
    }
}

void tui_selector::set_current_index(int direction) {
    if (options.empty()) return;

    if (direction == 1) { // 向下移动
        current_index = (current_index + 1) % options.size();
    } else if (direction == -1) { // 向上移动
        current_index = (current_index - 1 + options.size()) % options.size();
    }
}

int tui_selector::get_current_index() const {
    return current_index;
}

std::string tui_selector::get_current_option() const {
    if (current_index >= 0 && current_index < options.size()) {
        return options[current_index];
    }
    return "";
}

void tui_selector::draw() {
    if (is_bordered) {
        draw_border();
    }

    // 计算垂直方向的居中起始位置
    int start_y = pos_y + 1 + (height - 2 - options.size()) / 2;

    for (size_t i = 0; i < options.size(); ++i) {
        int option_y = start_y + i; // 每个选项的垂直位置
        const std::string& option_text = options[i];

        // 计算水平方向的居中位置
        int option_x = pos_x + 1 + (width - 2 - option_text.length()) / 2;

        // 高亮显示当前选中项
        if (i == current_index) {
            move_cursor(option_x, option_y);
            printf(">%s<", option_text.c_str());
        } else {
            move_cursor(option_x, option_y);
            printf(" %s ", option_text.c_str());
        }
    }
}

tui_label::tui_label(int pos_x, int pos_y, int width, int height, std::string name, const std::string& text)
    : tui_widght(pos_x, pos_y, width, height, false, name), text(text) {}

tui_label::~tui_label() {
    // 默认析构函数
}

void tui_label::set_text(const std::string& text) {
    this->text = text;
}

std::string tui_label::get_text() const {
    return text;
}

void tui_label::erase_text() {
    int text_x = pos_x + (width - text.length()) / 2;
    int text_y = pos_y + height / 2;

    move_cursor(text_x, text_y);
    for (int i = 0; i < text.size(); ++i) {
        printf(" ");
    }
    fflush(stdout);
}

void tui_label::draw() {
    // 计算文本的起始位置以实现居中显示
    int text_x = pos_x + (width - text.length()) / 2;
    int text_y = pos_y + height / 2;

    move_cursor(text_x, text_y);
    // 根据width限制文本长度
    printf("%.*s", width, text.c_str());
}