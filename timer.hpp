#ifndef TIMER_HPP
#define TIMER_HPP

#include <SDL2/SDL.h>

// A class that encapsulates timer functionality using SDL
class Timer {
private:
    Uint32 startTicks;  // Time when the timer started
    Uint32 pausedTicks; // Time stored when the timer was paused

    bool paused;  // Timer status flag: paused
    bool started; // Timer status flag: started

public:
    // Initializes variables
    Timer();

    // Starts the timer
    void start();

    // Stops the timer
    void stop();

    // Pauses the timer
    void pause();

    // Resumes the timer from paused state
    void resume();

    // Gets the timer's current tick count
    Uint32 getTicks();

    // Checks if the timer is started
    bool isStarted();

    // Checks if the timer is paused
    bool isPaused();
};

#endif // TIMER_HPP
