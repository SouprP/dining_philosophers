#include <headers/settings.h>

Settings::Settings(){
    this->set_min_eat_time(DEFAULT_MIN_EAT);
    this->set_max_eat_time(DEFAULT_MAX_EAT);

    this->set_min_think_time(DEFAULT_MIN_THINK);
    this->set_max_think_time(DEFAULT_MAX_THINK);

    this->set_death_time(DEFAULT_DEATH);
}

// EAT TIME
void Settings::set_min_eat_time(size_t miliseconds){
    this->min_eat_time = miliseconds;
}

void Settings::set_max_eat_time(size_t miliseconds){
    this->max_eat_time = miliseconds;
}

size_t Settings::get_eat_time(){
    return rand() % max_eat_time + min_eat_time;
}

// THINK TIME
void Settings::set_min_think_time(size_t miliseconds){
    this->min_think_time = miliseconds;
}

void Settings::set_max_think_time(size_t miliseconds){
    this->max_think_time = miliseconds;
}

size_t Settings::get_think_time(){
    return rand() % max_think_time + min_think_time;
}

// DEATH TIME
void Settings::set_death_time(size_t miliseconds){
    this->death_time = miliseconds;
}

size_t Settings::get_death_time(){
    return this->death_time;
}

/**
 * Condition variable mechaninc for PHILOSOPHER objects, explanation how it works below.
*/
void Settings::await_threads_ready(){
    // Philosopher object created and ready for start
    // lock Settings object so another thread
    // doesnt increase this at the same time
    std::unique_lock<std::mutex> phil_lock(cv_mutex);
    threads_ready++;
            
    // check if condition for start is reached
    // if not => wait till it is
    while(threads_ready < philosophers_count)
        cv.wait(phil_lock);
            
    // condition met, unlock the lock and unblock all threads
    cv.notify_all();
}

void Settings::set_philosopers_count(int count){
    if(count < 5)
        count = 5;
    
    if(count > 10)
        count = 10;
    
    this->philosophers_count = count;
}

unsigned int  Settings::get_philosopers_count(){
    return this->philosophers_count;
}

/**
 * Handling of args provided to the main function.
*/
void Settings::handle_args(size_t argc, char** argv){
    if(argc == 1){
        // return a message with args instead? or just let it be
        this->set_philosopers_count(rand() % + 5);
    }


    if(argc > 1)
        // we get arg number 1 and then jump 2 args forward, so +2
        //    1     2        3      4
        // -arg_1 value_1 -arg_2 value_2
        for(int i = 1; i < argc; i = i + 2){
            std::string cmd_arg(argv[i]); // get the arg
            uint64_t value = abs(std::atoi(argv[i + 1])); // get the arg value, convert it from string to int, should probably change to size_t

            if(!value)
                continue;

            //std::cout << cmd_arg << ", value: " << value << std::endl;

            // phil count arg
            if(!cmd_arg.compare("-phil"))
                this->set_philosopers_count(value);
            
            // death args
            else if(!cmd_arg.compare("-death"))
                this->set_death_time(value);

            // think args
            else if(!cmd_arg.compare("-think_min"))
                this->set_min_think_time(value);
            else if(!cmd_arg.compare("-think_max"))
                this->set_max_think_time(value);
            
            // eat args
            else if(!cmd_arg.compare("-eat_min"))
                this->set_min_eat_time(value);
            else if(!cmd_arg.compare("-eat_max"))
                this->set_max_eat_time(value);
        }
}   


