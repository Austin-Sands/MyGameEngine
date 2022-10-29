/*
Title:	Main.cpp
Author:	Austin Sands
Date:	10/03/2022
Purpose: Uses the SDL2 library to implement my own game engine for CSCI 43700. It utilizes a similar format to the ADT presented in class but with a specific mindset of working with a
	top-down game. I have had to learn how to use SDL2 so some of this will need to be refactored for efficiency once I get a better understanding.
Date Modified: 10/25/2022
 */

#include "Scene.h"
#include "Sprite.h"
#include <cstdio>

//create game scene
Scene gameScene = Scene();

//create variables to hold required media
//images
SDL_Texture* playerTexture = NULL;
SDL_Texture* enemyTexture = NULL;
SDL_Texture* playerProjectileTexture = NULL;

//initializes SDL components
void SDLInit();

//frees resources and quits SDL components
void close();

//load images at specified path to texture
SDL_Texture* textureFromFile(SDL_Renderer* renderer, std::string imagePath);

//loads required media 
void loadMedia();

//renders everything to screen
void draw();

//tasks to start and initialize scene
void initScene();

//initialize player for game
void initializePlayer();

//input handler returns a quit flag
bool handleInput();

//handle logic
void logic();

//initialize SDL components being used by the program
void SDLInit()
{
	//flag to indicate success/fail
	bool success = true;

	//initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		//if initialization fails, print error message to console
		printf("Unable to initialize SDL! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	//if SDL initialized, initialize additional SDL libraries
	else
	{
		//initialize PNG and JPG loading 
		if(!IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG))
		{
			printf("Unable to initialize SDL_image! SDL Error: %s\n", IMG_GetError());
		}
	} //end SDL initialization
}

void close()
{
	//close scene
	gameScene.free();

	//destroy any textures
	SDL_DestroyTexture(playerTexture);
	SDL_DestroyTexture(enemyTexture);
	SDL_DestroyTexture(playerProjectileTexture);

	//quit SDL and SDL libraries
	IMG_Quit();
	SDL_Quit();
}

SDL_Texture* textureFromFile(SDL_Renderer* renderer, std::string imagePath)
{
	//initialize texture to hold 
	SDL_Texture* loadedTexture = NULL;

	//load image at specified path
	loadedTexture = IMG_LoadTexture(renderer, imagePath.c_str());
	//check if loaded successfully
	if (loadedTexture == NULL)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Unable to create texture from %s\n", imagePath.c_str());
	}

	return loadedTexture;
}

void loadMedia()
{
	playerTexture = textureFromFile(gameScene.getRenderer(), "gfx/myPlayer.png");
	enemyTexture = textureFromFile(gameScene.getRenderer(), "gfx/myEnemy.png");
	playerProjectileTexture = textureFromFile(gameScene.getRenderer(), "gfx/myProjectile.png");
}

//render everything to screen
void draw()
{
	//call scene draw functions
	gameScene.draw();

	//render scene
	gameScene.render();
}

void initScene()
{
	//start frame cap timer
	gameScene.capStart();

	//prepare scene
	gameScene.prepare();

	//set player projectile texture
	gameScene.setPlayerProjectile(playerProjectileTexture);
}

void initializePlayer()
{
	Sprite* player = new Sprite(&gameScene, true, ENTITY, playerTexture);
	//set player to center of screen
	player->setPos(SCREEN_X_CENTER - (player->getWidth() / 2),	SCREEN_Y_CENTER - (player->getHeight() / 2));
}

bool handleInput()
{
	//get quit flag while getting input from scene
	bool quit = gameScene.doInput();

	//pass input to sprites

	return quit;
}

void logic()
{
	//bounding
	gameScene.bound();
}

//arguments passed per SDL documentation
int main(int argc, char * argv[])
{
	//flag for quitting
	bool quit = false;

	//initialize SDL
	SDLInit();

	//load required media
	loadMedia();

	//initialize player
	initializePlayer();

	//begin main game loop
	while(!quit)
	{
		//initialize scene
		initScene();

		//do scene input
		quit = handleInput();

		//handle logic
		logic();

		//draw scene to screen
		draw();

		gameScene.print();


		//cap frames
		gameScene.capFrames();
	}//end main game loop

	close();
	return 0;
}