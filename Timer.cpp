/*
Title:	Timer.cpp
Author:	Austin Sands
Date:	10/25/2022
Purpose: implementation file for Timer class for my game engine
 */

#include "Timer.h"

Timer::Timer()
{
	//initialize variables
	ticksStart = 0;
	ticksPaused = 0;

	//set status
	paused = false;
	started = false;

}

void Timer::start()
{
	//flag timer started
	started = true;

	//unpause in case paused
	paused = false;

	//get current clock time from sdl
	ticksStart = SDL_GetTicks64();

	//reset ticks paused to avoid data contamination
	ticksPaused = 0;
}

void Timer::stop()
{

	//flag timer not started, not paused
	paused = false;
	started = false;

	//clear tick variables
	ticksStart = 0;
	ticksPaused = 0;

}

void Timer::pause()
{
	//ensure timer is not paused and is running
	if(started && !paused)
	{
		//flag timer as paused
		paused = true;

		//set ticks when paused to pause ticks - started ticks
		ticksPaused = SDL_GetTicks64() - ticksStart;

		//reset ticks start to avoid data contamination
		ticksStart = 0;
	}
}

void Timer::unpause()
{
	//ensure timer is started and paused
	if(started && paused)
	{
		//flag timer unpaused
		paused = false;

		//set ticks started to current ticks - ticks when paused
		ticksStart = SDL_GetTicks64() - ticksPaused;

		//reset ticks when paused
		ticksPaused = 0;
	}
}

Uint64 Timer::getTicks()
{
	//if timer is not started no ticks to get
	if(started)
	{
		//if timer is paused return ticks when paused
		if (paused) { return ticksPaused; }
		//else return current ticks minus start ticks
		else { return SDL_GetTicks64() - ticksStart; }
	}
}