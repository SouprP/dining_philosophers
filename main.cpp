#include <iostream>

#include <headers/visualiser.h>
#include <headers/philosopher.h>
#include <headers/fork.h>
#include <headers/settings.h>


Settings settings;
std::vector<std::unique_ptr<Fork>> forks; // exclusive ownership of an object
std::vector<std::shared_ptr<Philosopher>> phils; // dynamic collection, removes automatically, no need for delete keyword


int main(int argc, char** argv){
    // get and set settings
    // for more info go to ./settings.cpp
    settings.handle_args(argc, argv);


    int count = settings.get_philosopers_count();

    // creating Fork objects
    for(int i = 0; i < count; i++){
        forks.emplace_back(new Fork());
    }

    // creating Philosopher objects
    for(int i = 0; i < count; i++){
        if(i != count - 1){
            phils.emplace_back(new Philosopher(i + 1, *forks[i], *forks[i+1], &settings));
        }
        else{
            phils.emplace_back(new Philosopher(i + 1, *forks[i], *forks[0], &settings));
        }
        
    }

    // set the phils vector for our PHILOSOPHER objects, as the vectors may differ in size
    // while emplacing_back the PHILOSOPHER objects
    // this is only useful if we use check_priority(), otherwise this could be ignored 
    for(auto obj : phils)
        obj->set_vector(phils);

    // init of Visualiser object
    Visualiser *visuals = new Visualiser(phils);

    // on keyboard input close the program and free memory
    getch();

    // clean up and threads joining
    delete visuals;
    phils.clear();
    forks.clear();

    return 0;
}