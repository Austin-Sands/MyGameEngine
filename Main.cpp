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
#include <SDL_ttf.h>
#include <sstream>
#include <iomanip>

//create game scene
Scene gameScene = Scene();

//create timer
Timer gameTimer = Timer();

//create variables to hold required media
//images
SDL_Texture* playerTexture = NULL;
SDL_Texture* enemyTexture = NULL;
SDL_Texture* playerProjectileTexture = NULL;
SDL_Texture* muzzleFlashTexture = NULL;
SDL_Texture* timerFontTexture = NULL;

//fonts
TTF_Font* timerFont = NULL;
//font stream
std::ostringstream fontTextStream;

//color for font
SDL_Color fontColor = { 140, 10, 30, 0 };
//rect for rendering font
SDL_Rect fontRenderRect = { (SCREEN_WIDTH / 2) - 100, 0, 200, 100 };

//initializes SDL components
void SDLInit();

//frees resources and quits SDL components
void close();

//load images at specified path to texture
SDL_Texture* textureFromFile(SDL_Renderer* renderer, std::string imagePath);

//load texture from TTF
SDL_Texture* loadFromText(std::string textureText);

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

//bool for quitting
bool quit;

//game over screen
void gameOver(int playerHealth);

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
			success = false;
		}

		//initialize TTF support
		if( TTF_Init() == -1)
		{
			printf("Unable to initialize SDL_ttf! SDL Error: %s\n", TTF_GetError());
			success = false;
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
	SDL_DestroyTexture(muzzleFlashTexture);
	SDL_DestroyTexture(timerFontTexture);

	//close fonts
	TTF_CloseFont(timerFont);
	timerFont = NULL;

	//quit SDL and SDL libraries
	IMG_Quit();
	TTF_Quit();
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

SDL_Texture* loadFromText(std::string textureText)
{
	//initialize texture to hold 
	SDL_Texture* loadedTexture = NULL;

	//render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(timerFont, textureText.c_str(), fontColor);
	if (textSurface == NULL)
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Unable to render font to surface! SDL ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		//create texture from surface
		loadedTexture = SDL_CreateTextureFromSurface(gameScene.getRenderer(), textSurface);
		if(loadedTexture == NULL)
		{
			SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Unable to create texture from surface! SDL Error: %s\n", SDL_GetError());
		}

		//free surface
		SDL_FreeSurface(textSurface);
	}

	return loadedTexture;
}

void loadMedia()
{
	playerTexture = textureFromFile(gameScene.getRenderer(), "gfx/myPlayer.png");
	enemyTexture = textureFromFile(gameScene.getRenderer(), "gfx/myEnemy.png");
	playerProjectileTexture = textureFromFile(gameScene.getRenderer(), "gfx/myProjectile.png");
	muzzleFlashTexture = textureFromFile(gameScene.getRenderer(), "gfx/muzzleFlash.png");

	//open font
	timerFont = TTF_OpenFont("gfx/HariPrimiantoro-owZdx.ttf", 28);
	if (timerFont == NULL) { SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_ERROR, "Unable to load font! SDL ttf Error: %s\n", TTF_GetError()); }
}

//render everything to screen
void draw()
{
		//draw background
		//create rect of whole screen
	SDL_Rect backgroundRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	//draw background
	SDL_SetRenderDrawColor(gameScene.getRenderer(), 61, 69, 33, 0);
	SDL_RenderFillRect(gameScene.getRenderer(), &backgroundRect);

	//call scene draw functions
	gameScene.draw();

	//clear font stream and replace
	fontTextStream.str("");
	fontTextStream << std::fixed << std::setprecision(2) << (180 - gameTimer.getTicks() / 1000.f);

	//draw timer font
	timerFontTexture = loadFromText(fontTextStream.str());
	SDL_RenderCopy(gameScene.getRenderer(), timerFontTexture, NULL, &fontRenderRect);

	//render scene
	gameScene.render();
}

void initScene()
{
	//start frame cap timer
	gameScene.capStart();

	//prepare scene
	gameScene.prepare();

	//set player projectile and muzzle texture
	gameScene.setPlayerProjectile(playerProjectileTexture, muzzleFlashTexture);
}

void initializePlayer()
{
	Sprite* player = new Sprite(&gameScene, true, ENTITY, playerTexture);
	//set player to center of screen
	player->setPos(SCREEN_X_CENTER - (player->getWidth() / 2),	SCREEN_Y_CENTER - (player->getHeight() / 2));

	//let scene know this is player
	gameScene.setPlayer(player);
}

bool handleInput()
{
	//get quit flag while getting input from scene
	bool quit = gameScene.doInput();

	//pass input to sprites

	return quit;
}

void gameOver(int playerHealth)
{
	gameScene.prepare();

	//draw background
	//create rect of whole screen
	SDL_Rect backgroundRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	//draw background
	SDL_SetRenderDrawColor(gameScene.getRenderer(), 0, 0, 0, 0);
	SDL_RenderFillRect(gameScene.getRenderer(), &backgroundRect);

	//clear font stream and replace
	fontTextStream.str("");

	//if survived, display that you win
	if(playerHealth > 0)
	{
		fontTextStream << "You have survived!";
	}
	else
	{
		fontTextStream << "You have died...";
	}

	//draw timer font
	timerFontTexture = loadFromText(fontTextStream.str());
	SDL_Rect gameOverRect = { SCREEN_WIDTH / 4, SCREEN_HEIGHT / 3, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3 };
	SDL_RenderCopy(gameScene.getRenderer(), timerFontTexture, NULL, &gameOverRect);

	//render to screen
	gameScene.render();

	bool exit = false;

	while(!exit)
	{
		SDL_Event e;

		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				exit = true;
				break;
			}
		}
	}
};

void logic()
{
	//handle enemies
	gameScene.doEnemies();

	//handle projectiles
	gameScene.doProjectiles();

	//spawn enemies
	gameScene.spawnEnemies(enemyTexture);

	//check for collisions
	gameScene.collisionCheck();

	//bounding player
	gameScene.bound();

	//if timer is out or player dead display end screen
	if (gameScene.getPlayer()->getHealth() == 0 || gameTimer.getTicks() >= 180000)
	{
		quit = true;
	}
}

//arguments passed per SDL documentation
int main(int argc, char * argv[])
{
	//flag for quitting
	quit = false;

	//initialize SDL
	SDLInit();

	//load required media
	loadMedia();

	//initialize player
	initializePlayer();

	//start game timer
	gameTimer.start();

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

		gameScene.print(); //TODO remove

		//cap frames
		gameScene.capFrames();

	}//end main game loop

	gameOver(gameScene.getPlayer()->getHealth());

	close();
	return 0;
}