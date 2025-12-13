#include <assert.h>
#include <sys/ioctl.h>
#include "main_window.h"


int main() {
    tui_init();
    struct winsize w;
    assert(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1);
    main_window *window= new main_window(0, 0, w.ws_col, w.ws_row, false, "main_window");
    // window->print_structure();
    window->run();
    // getchar();
    delete window;
    return 0;
}