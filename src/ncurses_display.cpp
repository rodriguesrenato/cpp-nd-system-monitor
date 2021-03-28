#include "ncurses_display.h"

#include <curses.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "format.h"
#include "system.h"

using std::string;
using std::to_string;

// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(float percent) {
    std::string result{"0%"};
    int size{50};
    float bars{percent * size};

    for (int i{0}; i < size; ++i) {
        result += i <= bars ? '|' : ' ';
    }

    string display{to_string(percent * 100).substr(0, 4)};
    if (percent < 0.1 || percent == 1.0) display = " " + to_string(percent * 100).substr(0, 3);
    return result + " " + display + "/100%";
}

// Display system informations
void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
    int row{0};
    mvwprintw(window, ++row, 2, ("OS: " + system.OperatingSystem()).c_str());
    mvwprintw(window, ++row, 2, ("Kernel: " + system.Kernel()).c_str());
    mvwprintw(window, ++row, 2, "CPU: ");
    wattron(window, COLOR_PAIR(1));
    mvwprintw(window, row, 10, "");
    wprintw(window, ProgressBar(system.Cpu().Utilization()).c_str());
    wattroff(window, COLOR_PAIR(1));
    mvwprintw(window, ++row, 2, "Memory: ");
    wattron(window, COLOR_PAIR(1));
    mvwprintw(window, row, 10, "");
    wprintw(window, ProgressBar(system.MemoryUtilization()).c_str());
    wattroff(window, COLOR_PAIR(1));
    mvwprintw(
        window, ++row, 2,
        ("Total Processes: " + Format::StrClean(to_string(system.TotalProcesses()), 8)).c_str());
    mvwprintw(window, ++row, 2,
              ("Running Processes: " + Format::StrClean(to_string(system.RunningProcesses()), 8))
                  .c_str());
    mvwprintw(window, ++row, 2, ("Up Time: " + Format::ElapsedTime(system.UpTime())).c_str());
    wrefresh(window);
}

// Display Process Table
void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes, WINDOW* window, int n) {
    int row{0};
    int const pid_column{2};
    int const user_column{9};
    int const cpu_column{18};
    int const ram_column{26};
    int const time_column{35};
    int const command_column{46};
    wattron(window, COLOR_PAIR(2));
    mvwprintw(window, ++row, pid_column, "PID");
    mvwprintw(window, row, user_column, "USER");
    mvwprintw(window, row, cpu_column, "CPU[%%]");
    mvwprintw(window, row, ram_column, "RAM[MB]");
    mvwprintw(window, row, time_column, "TIME+");
    mvwprintw(window, row, command_column, "COMMAND");
    wattroff(window, COLOR_PAIR(2));
    for (int i = 0; i < n; ++i) {
        mvwprintw(
            window, ++row, pid_column,
            Format::StrClean(to_string(processes[i].Pid()), user_column - pid_column - 1).c_str());
        mvwprintw(window, row, user_column,
                  Format::StrClean(processes[i].User(), cpu_column - user_column - 1).c_str());
        float cpu = processes[i].CpuUtilization() * 100;
        mvwprintw(window, row, cpu_column,
                  Format::StrClean(to_string(cpu).substr(0, 4), ram_column - cpu_column).c_str());
        mvwprintw(window, row, ram_column,
                  Format::StrClean(processes[i].Ram(), time_column - ram_column).c_str());
        mvwprintw(window, row, time_column, Format::ElapsedTime(processes[i].UpTime()).c_str());
        mvwprintw(
            window, row, command_column,
            Format::StrClean(processes[i].Command(), (int)window->_maxx - command_column).c_str());
    }
}

void NCursesDisplay::Display(System& system, int n) {
    initscr();      // start ncurses
    noecho();       // do not print input values
    cbreak();       // terminate ncurses on ctrl + c
    start_color();  // enable color

    int x_max{getmaxx(stdscr)};
    WINDOW* system_window = newwin(9, x_max - 1, 0, 0);
    WINDOW* process_window = newwin(3 + n, x_max - 1, system_window->_maxy + 1, 0);

    while (1) {
        init_pair(1, COLOR_BLUE, COLOR_BLACK);
        init_pair(2, COLOR_GREEN, COLOR_BLACK);
        box(system_window, 0, 0);
        box(process_window, 0, 0);
        DisplaySystem(system, system_window);
        DisplayProcesses(system.Processes(), process_window, n);
        wrefresh(system_window);
        wrefresh(process_window);
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    endwin();
}