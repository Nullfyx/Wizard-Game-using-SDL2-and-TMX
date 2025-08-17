#include "timer.hpp"

// Initializes variables
Timer::Timer() {
    startTicks = 0;
    pausedTicks = 0;
    paused = false;
    started = false;
}

// Starts the timer
void Timer::start() {
    started = true;
    paused = false;
    startTicks = SDL_GetTicks();
    pausedTicks = 0;
}

// Stops the timer
void Timer::stop() {
    started = false;
    paused = false;
    startTicks = 0;
    pausedTicks = 0;
}

// Pauses the timer
void Timer::pause() {
    if (started && !paused) {
        paused = true;
        pausedTicks = SDL_GetTicks() - startTicks;
        startTicks = 0;
    }
}

// Resumes the timer from paused state
void Timer::resume() {
    if (paused) {
        paused = false;
        startTicks = SDL_GetTicks() - pausedTicks;
        pausedTicks = 0;
    }
}

// Gets the timer's current tick count
Uint32 Timer::getTicks() {
    if (started) {
        if (paused)
            return pausedTicks;
        else
            return SDL_GetTicks() - startTicks;
    }
    return 0;
}

// Checks if the timer is started
bool Timer::isStarted() {
    return started;
}

// Checks if the timer is paused
bool Timer::isPaused() {
    return paused && started;
}
