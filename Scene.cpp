/*
Title:	Scene.cpp
Author:	Austin Sands
Date:	10/25/2022
Purpose: Implementation file for the Scene class for my game engine
 */

#include "Scene.h"

Scene::Scene()
{
	//initialize variables
	mWindow = NULL;
	mRenderer = NULL;
	framesCounted = 0;
	mousePos = { 0, 0 };

	//initialize keyboard
	for (int i = 0; i < MAX_KEYBOARD_KEYS; i++)
	{
		mKeyboard[i] = 0;
	}

	//initialize mouse button state
	leftClick = false;

	//initialize list variables
	entityHead = NULL;
	entityTail = entityHead;

	projectileHead = NULL;
	projectileTail = projectileHead;

	//initialize projectile images
	playerProjectileTexture = NULL;

	//create window
	mWindow = SDL_CreateWindow("Simple Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_FLAGS);
	//check if creation failed
	if(mWindow == NULL)
	{
		printf("Unable to create window! SDL Error: %s\n", SDL_GetError());
	}
	//if success, create renderer
	else
	{
		mRenderer = SDL_CreateRenderer(mWindow, -1, RENDER_FLAGS);
		if(mRenderer == NULL)
		{
			printf("Unable to create renderer! SDL Error: %s\n", SDL_GetError());
		}
	}
}

Scene::~Scene()
{
	free();
}

void Scene::prepare()
{
	//set background to white
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	//clear renderer
	SDL_RenderClear(mRenderer);
}

void Scene::render()
{
	//displays renderer onto screen
	SDL_RenderPresent(mRenderer);
}

void Scene::free()
{
	//free window and renderer if they exist
	if(mRenderer != NULL)
	{
		SDL_DestroyRenderer(mRenderer);
	}
	mRenderer = NULL;

	if(mWindow != NULL)
	{
		SDL_DestroyWindow(mWindow);
	}
	mWindow = NULL;

	//iterate through lists and free resources
	Sprite* temp = NULL;

	while(entityHead != NULL)
	{
		//set temp to current head
		temp = entityHead;

		//set new head to current head next
		entityHead = entityHead->next;

		//free resource of old head
		temp->free();
	}

	while (projectileHead != NULL)
	{
		//set temp to current head
		temp = projectileHead;

		//set new head to current head next
		projectileHead = projectileHead->next;

		//free resource of old head
		temp->free();
	}
}

void Scene::doKeyDown(SDL_KeyboardEvent* e)
{
	//check if not repeat 
	if(e->repeat == 0 && e->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		//set keyboard array at the pressed key to 1
		mKeyboard[e->keysym.scancode] = 1;
	}
}

void Scene::doKeyUp(SDL_KeyboardEvent* e)
{
	//check if not repeat
	if(e->repeat == 0 && e->keysym.scancode < MAX_KEYBOARD_KEYS)
	{
		mKeyboard[e->keysym.scancode] = 0;
	}
}

void Scene::doMouseDown(SDL_MouseButtonEvent* e)
{
	//check if left click
	if(e->button == SDL_BUTTON_LEFT)
	{
		leftClick = true;
	}
}

void Scene::doMouseUp(SDL_MouseButtonEvent* e)
{
	//check if left click
	if(e->button == SDL_BUTTON_LEFT)
	{
		leftClick = false;
	}
}

//mouse movement handler
void Scene::setMousePos()
{
	//get mouse position and set mouse position variables
	SDL_GetMouseState(&mousePos.x, &mousePos.y);
}//end setMousePos

bool Scene::doInput()
{
	//quit flag
	bool quit = false;

	//create event handler
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_QUIT:
			quit = true;
			break;

		case SDL_KEYDOWN:	//pass to handler on key down
			doKeyDown(&e.key);
			break;

		case SDL_KEYUP:
			doKeyUp(&e.key);	//pass to handler on key up
			break;

		case SDL_MOUSEMOTION:	
			setMousePos();
			break;

		case SDL_MOUSEBUTTONDOWN:
			doMouseDown(&e.button);
			break;

		case SDL_MOUSEBUTTONUP:
			doMouseUp(&e.button);
			break;

		default:
			break;
		}
	}

	//iterate through entity sprites list and handle input
	for(Sprite* current = entityHead; current != NULL; current = current->next)
	{
		current->doPlayer();
	}

	return quit;
}

void Scene::capStart()
{
	capTimer.start();
}

void Scene::capFrames()
{
	//add this frame to counted frames
	++framesCounted;

	//get current ticks
	Uint64 frameTicks = capTimer.getTicks();

	//check if frame ticks are less than constant SCREEN_TICKS_PER_FRAME
	if(frameTicks < SCREEN_TICKS_PER_FRAME)
	{
		//wait until constant
		SDL_Delay(SCREEN_TICKS_PER_FRAME - frameTicks);
	}
}

//add sprite to tail of list. Requires pointer to a sprite, sprite type int 0 = ENTITY 1 = PROJECTILE
void Scene::addSprite(Sprite* sprite, int spriteType)
{
	//determine which list to add to
	if(spriteType == ENTITY)
	{
		//check if list is empty
		if(entityHead == NULL)
		{
			//make new sprite head of list
			entityHead = sprite;

			//make single sprite tail as well
			entityTail = entityHead;
		}
		else
		{
			//if there are already sprites in list, add new sprite to tail
			//point next of current tail to new sprite
			entityTail->next = sprite;

			//set tail of list to now be new sprite
			entityTail = sprite;
		}
	}
	else if(spriteType == PROJECTILE)
	{
		//check if list is empty
		if (projectileHead == NULL)
		{
			//make new sprite head of list
			projectileHead = sprite;

			//make single sprite tail as well
			projectileTail = projectileHead;
		}
		else
		{
			//if there are already sprites in list, add new sprite to tail
			//point next of current tail to new sprite
			projectileTail->next = sprite;

			//set tail of list to now be new sprite
			projectileTail = sprite;
		}
	}
}

void Scene::draw()
{

	//iterate through entity sprite list and draw them to renderer
	for (Sprite* current = entityHead; current != NULL; current = current->next)
	{
		current->draw();
	}

	Sprite* previous = NULL;
	for (Sprite* current = projectileHead; current != NULL; current = current->next)
	{
		current->drawProjectiles();

		/*
		 * Having an issue with bounding the projectiles. Getting unexpected behavior when trying to
		 * delete objects in projectile list. I've been working on this portion for so long and it made the project late.
		 * Pretty demoralizing.
		 */
	//	if(current->getX() - current->getWidth() < 0)
	//	{
	//	}
	}
}
