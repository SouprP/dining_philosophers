#include <headers/visualiser.h>

Visualiser::Visualiser(std::vector<std::shared_ptr<Philosopher>> philosophers){
    this->philosophers = philosophers;
    setlocale(LC_ALL, "");

    // initialization of screen
    initscr();
    curs_set(0); // dont let the cursor show on screen

    // setup of windows
    //win = newwin(HEIGHT, WIDTH, 0, 0);
    // args -newwin(height of window, width of win, 
    // Y cord where to place, X cord where to place)
    info_win = newwin(INFO_HEIGH, INFO_WIDTH, 0, 0);

    id_win = newwin(WIN_HEIGHT, 10, INFO_HEIGH, 0);
    state_win = newwin(WIN_HEIGHT, 20, INFO_HEIGH, 10);
    perc_win = newwin(WIN_HEIGHT, 10, INFO_HEIGH, 30);
    fork_win = newwin(WIN_HEIGHT, 10, INFO_HEIGH, 40);
    refresh();

    // setup of windows boxes
    box(info_win, 0, 0);
    box(id_win, 0, 0);
    box(state_win, 0, 0);
    box(perc_win, 0, 0);
    box(fork_win, 0, 0);
    wrefresh(info_win);
    wrefresh(id_win);
    wrefresh(state_win);
    wrefresh(perc_win);
    wrefresh(fork_win);
    
    noecho(); // dont print getch() output
    attron(A_BOLD); // make the text BOLD


    // init of color pairs for later use
    // BACKGROUND is defined ./headers/visualiser.h
    // init_pair(pair number, foreground color, background color)
    start_color();
    init_pair(1, COLOR_WHITE, BACKGROUND); // default
    init_pair(2, COLOR_CYAN, BACKGROUND); // thinking
    init_pair(3,COLOR_GREEN, BACKGROUND); // eating
    init_pair(4, COLOR_MAGENTA, BACKGROUND); // waiting
    init_pair(5, COLOR_RED, BACKGROUND); // dead
    init_pair(6, COLOR_YELLOW, BACKGROUND); // forks 

    program_start_time = std::chrono::high_resolution_clock::now();

    // just left it to be here, altough it probably should be removed from here
    //write_info(); // removed

    // start the loop
    should_run = true;
    visual_thread = std::thread(&Visualiser::update, this);
    //visual_thread = std::thread(&Visualiser::init, this);

}

Visualiser::~Visualiser(){
    should_run = false;
    visual_thread.join();

    // delete windows
    delwin(info_win);
    delwin(id_win);
    delwin(state_win);
    delwin(perc_win);
    delwin(fork_win);
    delwin(stdscr); // ncurses default main window
    endwin();
}

/**
 * This function is a remnant of an old code, It's not used and should be deleted later.
*/
void Visualiser::init(){
    write_info();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / HZ));
    if(!should_run)
        return;

    init();
}

/**
 * This function writes everything to the screen and handles refresh rate as well as the self recurversing loop.
*/
void Visualiser::update(){
    write_info();

    int row = 0;
    for(auto obj : philosophers){
        write_id(row, obj);
        write_state(row, obj);
        write_progress(row, obj);
        write_fork_use(row, obj);
        row++;
    }

    // if we dont use sleep_for the this thread, it will throw Segmented Fault exception
    // you could say it makes it so that there is a refresh rate
    std::this_thread::sleep_for(std::chrono::milliseconds(1000 / HZ));

    // check if Visualiser object was deleted and told to stop running
    if(!should_run)
        return;
    
    update();
}

/**
 * Gets program runtime in seconds.
 * 
 * @return seconds as size_t as they are going to be >= 0.
*/
size_t Visualiser::get_runtime(){
    // gets the diff beetwen 2 times
    auto time = std::chrono::duration<double>(std::chrono::high_resolution_clock::now() 
    - program_start_time);

    // converts time to miliseconds and returns it devided by 1000
    auto time_conv = std::chrono::duration_cast<std::chrono::milliseconds>(time);
    return time_conv.count() / 1000;
}

/**
 * Writes info about the programm and other stuff to the screen.
*/
void Visualiser::write_info(){
    int x = getmaxx(info_win);
    int y getmaxy(info_win);

    std::string prog_name = "Dining Philosophers";
    std::string name = "Igor Potyrala - 272518";
    std::string freq = "Frequency:: %dHz";
    std::string time = "Time elapsed: %ds";

    mvwprintw(info_win, 1, (int) (x / 2) - prog_name.length() / 2, prog_name.c_str());
    mvwprintw(info_win, 2, (int) (x / 2) - name.length() / 2, name.c_str());

    mvwprintw(info_win, 4, (int) (x / 2) - freq.length() / 2, freq.c_str(), HZ);
    mvwprintw(info_win, 5, (int) (x / 2) - time.length() / 2, time.c_str(), get_runtime());
    wrefresh(info_win);
}

/**
 * Handles writing PHILOSOPHER objects ID to the screen.
*/
void Visualiser::write_id(int row, std::shared_ptr<Philosopher> obj){
    std::string id_s = std::to_string(obj->get_id());
    mvwprintw(id_win, PADDING_Y*row + 1, 4, id_s.c_str());
    wrefresh(id_win);
}

/**
 * Handles writing PHILOSOPHER objects states to the screen.
*/
void Visualiser::write_state(int row, std::shared_ptr<Philosopher> obj){
    int X_PADDING = 5;

    switch (obj->get_state())
        {
        case State::THINKING:
            wattron(state_win, COLOR_PAIR(2));
            mvwprintw(state_win, PADDING_Y*row + 1, X_PADDING,
             obj->get_state_str());

            wattroff(state_win, COLOR_PAIR(2));
            break;

        case State::EATING:
            wattron(state_win, COLOR_PAIR(3));
            mvwprintw(state_win, PADDING_Y*row + 1, X_PADDING,
             obj->get_state_str());

            wattroff(state_win, COLOR_PAIR(3));
            break;

        case State::AWAITING_FORKS:
            wattron(state_win, COLOR_PAIR(4));
            mvwprintw(state_win, PADDING_Y*row + 1, X_PADDING,
             obj->get_state_str()) ;
            
            wattroff(state_win, COLOR_PAIR(4));
            break;

        case State::DEAD:
            wattron(state_win, COLOR_PAIR(5));
            mvwprintw(state_win, PADDING_Y*row + 1, X_PADDING,
             obj->get_state_str()) ;
            
            wattroff(state_win, COLOR_PAIR(5));
            break;
        
        default:
            break;
        }

    wrefresh(state_win);
}

/**
 * Handles writing PHILOSOPHER objects current interaction progress to the screen.
*/
void Visualiser::write_progress(int row, std::shared_ptr<Philosopher> obj){
    //std::string perc_s = std::to_string(obj->get_id());
    mvwprintw(perc_win, PADDING_Y*row + 1, 3, "     ");
    mvwprintw(perc_win, PADDING_Y*row + 1, 3, "%d%%%", obj->get_perc());
    wrefresh(perc_win);
}

/**
 * Handles writing PHILOSOPHER objects current forks that they are holding to the screen.
 * I have no idea how to simulate them in any other way, maybe through IDs? but that would look like crap
*/
void Visualiser::write_fork_use(int row, std::shared_ptr<Philosopher> obj){
    // ⑂ 
    // 🍴
    // u\2442
    const char* emoji = "⑂";
    //const char* emoji = "🍴";

    wattron(fork_win, 6);
    mvwprintw(fork_win, PADDING_Y*row + 1, 3 ,"   ");
    int forks = obj->get_fork_count();

    if(forks == 1)
        mvwprintw(fork_win, PADDING_Y*row + 1, 3, "%s", emoji);
    
    if(forks == 2)
        mvwprintw(fork_win, PADDING_Y*row + 1, 3, "%s %s", emoji, emoji);
    


    wattroff(fork_win, 6);
    wrefresh(fork_win);
}