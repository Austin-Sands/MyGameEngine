/*
Title:	Timer.h
Author:	Austin Sands
Date:	10/25/2022
Purpose: header file for Timer class for my game engine.
TODO pause function implementation
 */

#pragma once
#ifndef TIMER_H
#define TIMER_H

#include <SDL.h>

class Timer
{
public:
	//initialize variables
	Timer();

	//clock functions
	void start();
	void stop();
	void pause();
	void unpause();

	//timer current time getter
	Uint64 getTicks();

	//timer status getters
	bool isStarted() const { return started; }
	bool isPaused() const { return paused; }

private:
	//the clock time when started
	Uint64 ticksStart;

	//clock time when paused
	Uint64 ticksPaused;

	//timer status
	bool paused;
	bool started;
};


#endif