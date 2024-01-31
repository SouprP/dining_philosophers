#ifndef SETTINGS_H
#define SETTINGS_H

#include <iostream>
#include <vector>
#include <chrono>
#include <condition_variable>

#define DEFAULT_MIN_EAT 2000
#define DEFAULT_MAX_EAT 3000
#define DEFAULT_MIN_THINK 2000
#define DEFAULT_MAX_THINK 5000
#define DEFAULT_DEATH 20000

class Settings{
    private:
        std::condition_variable cv;
        std::mutex cv_mutex;
        int threads_ready = 0;
        int philosophers_count;

        size_t max_eat_time;
        size_t min_eat_time ;

        size_t max_think_time;
        size_t min_think_time;

        size_t death_time;

        std::vector<std::string> ARGS{"-phil", "-death_min", 
        "-death_max", "-think_min"};
        
    public:
        void set_min_eat_time(size_t miliseconds);
        void set_max_eat_time(size_t miliseconds);
        size_t get_eat_time();

        void set_min_think_time(size_t miliseconds);
        void set_max_think_time(size_t miliseconds);
        size_t get_think_time();

        void set_death_time(size_t miliseconds);
        size_t get_death_time();

        void set_philosopers_count(int count);
        unsigned int get_philosopers_count();
        void await_threads_ready();

        void handle_args(size_t argc, char** argv);
        
        Settings();
};

#endif