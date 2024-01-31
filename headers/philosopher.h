#ifndef PHILOSOPHER_H
#define PHILOSOPHER_H

#include <thread>
#include <chrono>
#include <iostream>
#include <headers/fork.h>
#include <headers/settings.h>

enum class State{
    PAUSED,
    THINKING,
    EATING,
    AWAITING_FORKS,
    DEAD,
};

class Philosopher {
    private:
        int id;
        State state;
        std::thread local_thread;
        bool should_run;
        uint8_t forks;

        Settings* settings;
        Fork* left_fork;
        Fork* right_fork;

        std::vector<std::shared_ptr<Philosopher>> philosophers;

        // time stamps
        std::chrono::high_resolution_clock::time_point starvation_start;
        std::chrono::high_resolution_clock::time_point inter_start;
        std::chrono::high_resolution_clock::time_point inter_end;

        //inline const char* to_string(); 
        void think();
        void eat();
        void check_starvation();
        bool check_priority();

    public:
        void start();
        State get_state();
        const char* get_state_str();
        int get_id();
        void set_vector(std::vector<std::shared_ptr<Philosopher>> philosophers);

        std::chrono::high_resolution_clock::time_point get_inter_start();
        std::chrono::high_resolution_clock::time_point get_inter_end();
        void set_inter_time(std::chrono::milliseconds time);
        size_t time_till_death();
        uint8_t get_perc();
        uint8_t get_fork_count();

        Philosopher(int id, Fork& left_fork, Fork& right_fork, Settings* settings);
        ~Philosopher();
};

#endif