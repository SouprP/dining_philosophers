# dining_philosophers

Solution to the dinings philosophers problem using **C++** and **std::thread** with **ncurses**.
 
<img src="images/philosophers.gif">


<br></br>
## Running a simulation

In order to run the programe you first need to compile it using [GNU](https://gcc.gnu.org) (or any other compiler, below is an example of how to compile it with GNU).

If you don't want to support unicode characters then you can include **lncurses** instead of **lncursesw**.
```
g++ -I ./  main.cpp philosopher.cpp settings.cpp visualiser.cpp -lncursesw -w
```

---
Then you can run the programm with or without args.
```
# Normal run, the number of philosophers will be random.
./a.out

# Sets the number of philosopher, the value must be between 5 and 10.
./a.out -phil X

# Sets the time (miliseconds) after which the philosopher will die.
./a.out -death X

# Sets the MINIMUM time of thinking (miliseconds).
./a.out -think_min X

# Sets the MAXIMUM time of thinking (miliseconds).
./a.out -think_max X

# Sets the MINIMUM time of eating (miliseconds).
./a.out -eat_min X

# Sets the MAXIMUM time of eating (miliseconds).
./a.out -eat_max X
```