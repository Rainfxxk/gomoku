#pragma once
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

void restore_terminal();
void tui_init();

class tui_widght {

public:
static void move_cursor(int x, int y);
static void hide_cursor(bool hide);
static void clear_screen();

public:
    tui_widght* parent;
    tui_widght* children;
    tui_widght* next;
    int width;
    int height;
    int pos_x;
    int pos_y;
    bool is_bordered;
    std::string name;

protected:
    void draw_border(const char** border_string, int x, int y, int width, int height);

public:
    tui_widght();
    tui_widght(int width, int height, bool is_bordered);
    tui_widght(int width, int height, bool is_bordered, std::string name);
    tui_widght(int pos_x, int pos_y, int width, int height, bool is_bordered);
    tui_widght(int pos_x, int pos_y, int width, int height, bool is_bordered, std::string name);
    virtual ~tui_widght();
    void draw_border(const char** border_string = nullptr);
    virtual void draw();
    void add(tui_widght* child);
    virtual void print_structure(int indent = 0);
};

class tui_layout : public tui_widght {
public:
    enum tui_layout_type{
        LAYOUT_HORIZONTAL = 0,
        LAYOUT_VERTICAL = 1
    };

private:
    /* data */
    tui_layout_type layout_type;
public:
    tui_layout(tui_layout_type layout_type);
    tui_layout(tui_layout_type layout_type, std::string name);
    ~tui_layout();
    void draw() override;
    void add(tui_widght* child, int radio);
    void add(tui_widght* child);
};

class tui_stack_widget : public tui_widght {
public:
    tui_stack_widget(int pos_x, int pos_y, int width, int height, bool is_bordered, std::string name);
    ~tui_stack_widget();

    void add(tui_widght* widget);
    void set_current_index(int index);
    int get_current_index() const;
    tui_widght* get_current_widget() const;
    void draw() override;
    void print_structure(int indent = 0) override;

private:
    std::vector<tui_widght*> widgets; // 存储子窗口
    int current_index; // 当前显示的子窗口索引
};

class tui_selector : public tui_widght {
public:
    tui_selector(int pos_x, int pos_y, int width, int height, bool is_bordered, std::string name);
    ~tui_selector();

    void add_option(const std::string& option);
    void set_current_index(int direction);
    int get_current_index() const;
    std::string get_current_option() const;
    void draw() override;

private:
    std::vector<std::string> options; // 选项列表
    int current_index; // 当前选中项的索引
};

class tui_label : public tui_widght {
public:
    tui_label(int pos_x, int pos_y, int width, int height, std::string name, const std::string& text);
    ~tui_label();
    void set_text(const std::string& text);
    std::string get_text() const;
    void erase_text();
    void draw() override;
private:
    std::string text;
    int old_length;
};