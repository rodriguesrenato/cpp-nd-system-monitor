#include "ncurses_display.h"
#include "system.h"

// Initialize system class and display it using NCurseDisplay
int main() {
    System system;
    NCursesDisplay::Display(system);
}