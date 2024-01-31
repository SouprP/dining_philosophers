#include <headers/philosopher.h>

Philosopher::Philosopher(int id, Fork& left_fork, Fork& right_fork, Settings *settings){
    this->id = id;
    this->left_fork = &left_fork;
    this->right_fork = &right_fork;
    this->settings = settings;

    should_run = true;
    forks = 0;
    local_thread = std::thread(&Philosopher::start, this);
}

Philosopher::~Philosopher(){
    should_run = false;
    local_thread.join();
}

/**
 * Main loop of the whole PHILOSOPER object.
*/
void Philosopher::start(){
    // await all philosophers (block the thread)
    // when all ready unblocks the thread and goes forward
    // to the main loop
    settings->await_threads_ready();

    // if we dont check for should_run it would not let the thread join the main thread and continue endlessly
    while(should_run){
        if(state == State::DEAD){
            should_run = false;
            return;
        }
        think();
        eat();
    }
}

/**
 * This function simulates eating, more description below.
*/
void Philosopher::eat(){
    state = State::AWAITING_FORKS;
    starvation_start = std::chrono::high_resolution_clock::now();

    // check if any other philosopher near us needs the forks more than us
    while(!check_priority())
        std::this_thread::yield();

    // first we check using mtx.lock() if we can take the forks, if not it will
    // await till the ownership is able to be taken and then take it
    left_fork->fork_mtx.lock();
    forks = 1;
    right_fork->fork_mtx.lock();
    forks = 2;

    // after we took both forks, check if we starved to death, if yes => unlock the mutexes and then 
    // return so that we can go out of the loop
    check_starvation();
    if(state == State::DEAD){
        left_fork->fork_mtx.unlock();
        right_fork->fork_mtx.unlock();
        return;
    }

    // we are not starved, start the eating process
    state = State::EATING;

    // get random time from settings and make the thread sleep ( eating simulation )
    auto time = std::chrono::milliseconds(settings->get_eat_time());
    set_inter_time(time);
    std::this_thread::sleep_for(time);

    // eating process finished, unlock the fork mutexes
    left_fork->fork_mtx.unlock();
    right_fork->fork_mtx.unlock();
    forks = 0;

}

/**
 * This function simulates the thinking process, by making this PHILOSOPER's object thread sleep.
*/
void Philosopher::think(){
    state = State::THINKING;
    forks = 0;

    // get random time from settings and make the thread sleep (sleeping simulation )
    auto time = std::chrono::milliseconds(settings->get_think_time());
    set_inter_time(time);
    std::this_thread::sleep_for(time);

}

/**
 * This function returns the current state of a PHILOSOPHER object.
 * 
 * @return State object as const char*.
*/
const char* Philosopher::get_state_str(){
    switch(state){
        case State::PAUSED:          return  "PAUSED  ";
        case State::THINKING:        return  "THINKING";
        case State::AWAITING_FORKS:  return  "AWAITING";
        case State::EATING:          return  "EATING  ";
        case State::DEAD:            return  "DEAD    ";
        default:                     return  "ERROR   ";
    }
}

/**
 * This function returns the current state of a PHILOSOPHER object.
 * 
 * @return State object from enum.
*/
State Philosopher::get_state(){
    return state;
}

/**
 * This function returns a time_point of when the current interaction started.
*/
std::chrono::high_resolution_clock::time_point Philosopher::get_inter_start(){
    return inter_start;
}

/**
 * This function returns a time_point of when the current interaction ends.
*/
std::chrono::high_resolution_clock::time_point Philosopher::get_inter_end(){
    return inter_end;
}

/**
 * Gets current interaction completion percentage, if the current state AWAITING_FORKS or DEAD, return 0.
 * 
 * @return percantage as unit8_t as the values dont surpass 100 and dont go below 0.
*/
uint8_t Philosopher::get_perc(){
    if(state == State::AWAITING_FORKS || state == State::DEAD)
        return 0;

    auto inter_time = std::chrono::duration<double>(get_inter_end() - get_inter_start());
    auto time_passed = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - get_inter_start());

    return (time_passed / inter_time) * 100;
}

int Philosopher::get_id(){
    return id;
}

/**
 * Sets the start of interaction and end of interaction variables times.
 * @attention inter_start - current time.
 * @attention inter_end - current time + randomized interaction time from SETTINGS object.
*/
void Philosopher::set_inter_time(std::chrono::milliseconds time){
    inter_start = std::chrono::high_resolution_clock::now();
    inter_end = inter_start + time; 
}

// 
/**
 * Checks if the philosopers waiting time for forks surpassed the DEATH TIME, 
 * if yes, set their state as DEAD.
*/
void Philosopher::check_starvation(){
    auto time_pass = std::chrono::duration_cast<std::chrono::milliseconds>
    (std::chrono::high_resolution_clock::now() - starvation_start).count();

    if(time_pass > settings->get_death_time())
        state = State::DEAD;
}

/**
 * This function checks if this PHILOSOPHER object has priority over the neighbouring PHILOSOPER objects,
 * it uses time_till_death() as a check if this process should be prioritized over others.
 * 
 * @return true if has priority
 * @return false if doesnt have priority
*/
bool Philosopher::check_priority(){
    int behind, forward;
    int real_id = id - 1;
    int indexes = philosophers.size() - 1;

    if(real_id == 0){
        forward = real_id + 1;
        behind = indexes;
    }
    else if(real_id == indexes){
        forward = 0;
        behind = real_id - 1;
    }
    else{
        forward = real_id + 1;
        behind = real_id - 1;
    }

    auto obj_f = philosophers.at(forward);
    auto obj_b = philosophers.at(behind);

    if((obj_f->get_state() == State::AWAITING_FORKS && obj_f->time_till_death() > this->time_till_death()) 
        || (obj_b->get_state() == State::AWAITING_FORKS && obj_b->time_till_death() > this->time_till_death()))
        return false; // should give priority to others

    return true; // has priority
}

/**
 * This function gets the time that have passed since the state AWAITING_FORKS was set, the name may be misleading.
 * 
 * @return time passed since the beggining of starvation.
*/
size_t Philosopher::time_till_death(){
    auto time = std::chrono::high_resolution_clock::now() - starvation_start;
    auto time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(time).count();

    return time_passed / 1000;
}

/**
 * Sets the phils vector object, so the check_priority() fuctions knows what neighbouring philosopers it has to check.
*/
void Philosopher::set_vector(std::vector<std::shared_ptr<Philosopher>> philosophers){
    this->philosophers = philosophers;
}

/**
 * Gets the current fork amount the PHILOSOPHER object has.
*/
uint8_t Philosopher::get_fork_count(){
    return forks;
}