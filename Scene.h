/*
Title:	Scene.h
Author:	Austin Sands
Date:	10/25/2022
Purpose: header file for Scene class for my game engine. REQUIRES SPRITE CLASS
 */

#pragma once
#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <ctime>
#include "Timer.h"
#include "Sprite.h"

//constants for screen size
//change these for desired screen sizes, keyboard settings, render/window flags etc.
const int SCREEN_WIDTH = 1280;
const int SCREEN_X_CENTER = SCREEN_WIDTH / 2;
const int SCREEN_HEIGHT = 960;
const int SCREEN_Y_CENTER = SCREEN_HEIGHT / 2;
const int WINDOW_FLAGS = SDL_WINDOW_INPUT_GRABBED;
const int RENDER_FLAGS = SDL_RENDERER_ACCELERATED;
const int SCREEN_FPS = 30;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;
const int MAX_KEYBOARD_KEYS = 256;
const float ENEMY_SPEED_BASE = 6;
const int ENEMY_SPAWN_LIMIT = 25;

//forward declaration
class Sprite;

class Scene
{
public:
	//initialize variables
	Scene();

	//destroctor
	~Scene();

	//set background color and clear renderer
	void prepare();

	//renders to screen
	void render();

	//deallocates resources
	void free();

	//handlers for key inputs
	void doKeyDown(SDL_KeyboardEvent* e);
	void doKeyUp(SDL_KeyboardEvent* e);

	//handler for mouse button input
	void doMouseDown(SDL_MouseButtonEvent* e);
	void doMouseUp(SDL_MouseButtonEvent* e);

	//handle input
	bool doInput();

	//starter for screen cap timer
	void capStart();

	//cap frame rate
	void capFrames();

	//add sprite to scene sprite list
	void addSprite(Sprite* sprite, int spriteType);

	//set projectile and muzzle flash texture
	void setPlayerProjectile(SDL_Texture* projectileTexture, SDL_Texture* muzzleFlashTexture)
	{
		playerProjectileTexture = projectileTexture;
		playerMuzzleFlashTexture = muzzleFlashTexture;
	}

	//bound sprites
	void bound();

	//draw all sprites to renderer
	void draw();

	//handle projectiles
	void doProjectiles();

	//handle projectile collisions
	int projectileCollideEnemy(Sprite* projectile);

	//calculate if there was a collision
	int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);

	//handle enemies
	void doEnemies();

	//spawn enemies
	void spawnEnemies(SDL_Texture* enemyTexture);

	//check for collisions
	void collisionCheck();

	//getters
	int* getKeyboard() { return mKeyboard; }				//get keyboard state
	SDL_Renderer* getRenderer() const { return mRenderer; }	//get renderer
	SDL_Point getMousePos() const { return mousePos; }		//handler for mouse position
	bool getMouseLeft() const { return leftClick; }			//get mouse button state
	SDL_Point getPlayerPos();								//return players position
	void setPlayer(Sprite* playerSprite);					//sets player object
	Sprite* getPlayer() { return player; }	//returns player health
	SDL_Texture* getPlayerProjectile() const { return playerProjectileTexture; }	//get player projectile
	SDL_Texture* getMuzzleFlash() const { return playerMuzzleFlashTexture; }	//get player projectile
	int getEnemyCount() const { return enemyCount; }		//get number of enemy entities
	void print();


private:

	//renderer
	SDL_Renderer* mRenderer;

	//window
	SDL_Window* mWindow;

	//keyboard array
	int mKeyboard[MAX_KEYBOARD_KEYS];

	//mouse position
	void setMousePos();
	SDL_Point mousePos;

	//mouse button state
	bool leftClick;

	//scene timer to keep track of current time/time passed
	Timer sceneTimer;

	//timer for fps cap
	Timer capTimer;

	//integer to count frames for frame cap
	int framesCounted;

	//heads are pointers to prevent circular dependency
	//hold items for players/enemies sprite linked list
	Sprite* entityHead, * entityTail;

	//hold items for projectile sprite linked list
	Sprite* projectileHead, * projectileTail;

	//hold texture for projectile sprite
	SDL_Texture* playerProjectileTexture;

	//hold texture for muzzle flash
	SDL_Texture* playerMuzzleFlashTexture;

	//enemy spawn countdown
	float enemyCountdown;

	//generate random number for spawner
	int getRand();

	//number of enemies in scene
	int enemyCount;

	//player object
	Sprite* player;
};
#endif