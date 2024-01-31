#ifndef VISUALISER_H
#define VISUALISER_H

#include <vector>
#include <ncurses.h>
#include <thread>
#include <locale>
#include <headers/philosopher.h>

// color names and
#define BACKGROUND COLOR_BLACK

// window params
#define INFO_WIDTH 50 // previus 100
#define INFO_HEIGH 8
#define WIN_HEIGHT 30
#define PADDING_X 10
#define PADDING_Y 3
#define HZ 20

class Visualiser{
    private:
        // ncurses.h windows
        WINDOW* win; // main holder of windows - not used atm
        WINDOW* id_win;
        WINDOW* state_win;
        WINDOW* perc_win;
        WINDOW* fork_win;
        WINDOW* info_win;

        // threads handling
        std::thread visual_thread;
        bool should_run;

        std::vector<std::shared_ptr<Philosopher>> philosophers;

        // program runtime info 
        // we get chrone::....::now() - program_start_time and get the proram runtime
        std::chrono::high_resolution_clock::time_point program_start_time;
        
        void init();
        void update();

        // helper functions for writing to screen using mvwprintw()
        void write_info();
        void write_id(int row, std::shared_ptr<Philosopher> obj);
        void write_state(int row, std::shared_ptr<Philosopher> obj);
        void write_progress(int row, std::shared_ptr<Philosopher> obj);
        void write_fork_use(int row, std::shared_ptr<Philosopher> obj);

        // gets visualiser runtime
        size_t get_runtime();

    public:
        Visualiser(std::vector<std::shared_ptr<Philosopher>> philosophers);

        ~Visualiser();
};


#endif