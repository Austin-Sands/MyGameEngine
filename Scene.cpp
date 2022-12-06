/*
Title:	Scene.cpp
Author:	Austin Sands
Date:	10/25/2022
Purpose: Implementation file for the Scene class for my game engine
 */

#include "Scene.h"

Scene::Scene()
{
	//initialize randomizer
	srand( time(NULL) );

	//initialize enemy count
	enemyCount = 0;

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

	//initialize player
	player = NULL;

	//initialize list variables
	entityHead = NULL;
	entityTail = entityHead;

	projectileHead = NULL;
	projectileTail = projectileHead;

	//initialize projectile images
	playerProjectileTexture = NULL;

	//initialize enemy countdown
	enemyCountdown = 30;

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

//get a random number between 0 and 1 for spawner
int Scene::getRand()
{
	return rand() % 2;
}

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

void Scene::bound()
{
	//bound player
	for(Sprite* current = entityHead; current != NULL; current = current->next)
	{
		if(current->isPlayer())
		{
			//check horizontal bounds
			if(current->getX() < 0)
			{
				current->setPos(0, current->getY());
			}
			else if(current->getX() + current->getWidth() > SCREEN_WIDTH)
			{
				current->setPos(SCREEN_WIDTH - current->getWidth(), current->getY());
			}

			//check vertical bounds
			if(current->getY() < 0)
			{
				current->setPos(current->getX(), 0);
			}
			else if(current->getY() + current->getHeight() > SCREEN_HEIGHT)
			{
				current->setPos(current->getX(), SCREEN_HEIGHT - current->getHeight());
			}
		}
	}

	////bound projectiles
	//Sprite* previous = NULL;
	//Sprite* current = projectileHead;

	////check head
	//if(current != NULL && current == projectileHead && (current->getX() > SCREEN_WIDTH || current->getX() + current->getWidth() < 0 || current->getY() > SCREEN_HEIGHT || current->getY() + current->getHeight() < 0))
	//{
	//	projectileHead = current->next;

	//	delete current;
	//	return;
	//}
	//else
	//{
	//	while(current != NULL && !(current->getX() > SCREEN_WIDTH || current->getX() + current->getWidth() < 0 || current->getY() > SCREEN_HEIGHT || current->getY() + current->getHeight() < 0))
	//	{
	//		previous = current;
	//		current = current->next;
	//	}

	//	if (current == NULL)
	//	{
	//		return;
	//	}

	//	previous->next = current->next;

	//	delete current;
	//}
}

void Scene::draw()
{
	//iterate through entity sprite list and draw them to renderer
	for (Sprite* current = entityHead; current != NULL; current = current->next)
	{
		current->draw();
	}

	for (Sprite* current = projectileHead; current != NULL; current = current->next)
	{
		current->draw();
	}
}

void Scene::doProjectiles()
{
//bound projectiles
	Sprite* previous = NULL;
	Sprite* current = NULL;

	for (current = projectileHead; current != NULL; current = current->next)
	{
		//move projectiles
		current->moveSprite();

		//check if projectile is colliding or out of bounds
		if(projectileCollideEnemy(current) || current->getX() > SCREEN_WIDTH || current->getX() + current->getWidth() < 0 || current->getY() > SCREEN_HEIGHT || current->getY() + current->getHeight() < 0)
		{
			//check if head or tail
			if (current == projectileTail)
			{
				projectileTail = previous;
			}
			if(current == projectileHead)
			{
				//if head, set new head to next node and continue through iterations
				projectileHead = current->next;
				continue;
			}

			//set previous node's next to current nodes next, eliminating current from the list
			previous->next = current->next;

			//free currents data
			current->free();

			//set current to the previous node
			current = previous;
		}

		//set previous node to current node before iterating
		previous = current;
	}
}

int Scene::projectileCollideEnemy(Sprite* projectile)
{
	Sprite* current = NULL;

	for(current = entityHead; current != NULL; current = current->next)
	{
		//if entity is not player and collides with projectile
		if (!current->isPlayer() && collision(projectile->getX(), projectile->getY(), projectile->getWidth(), projectile->getHeight(), current->getX(), current->getY(), current->getWidth(), current->getHeight()))
		{
			//set colliding entity health to 0
			current->setHealth(0);

			return 1;
		}
	}

	return 0;
}

int Scene::collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
		//checks for a sprites image rect to be within the second sprites space. X AND Y positions must overlap
		return (std::max(x1, x2) < std::min(x1 + w1, x2 + w2) && (std::max(y1, y2) < std::min(y1 + w1, y2 + w2)));
}


void Scene::doEnemies()
{
	Sprite* previous = entityHead;

	for(Sprite* current = entityHead->next; current != NULL; current = current->next)
	{
		//if enemy
		if(!current->isPlayer())
		{
			//calculate speed
			//TODO change with battery
			float enemySpeed = ENEMY_SPEED_BASE + (rand() % 6);

			//move enemies
			current->moveEnemy(enemySpeed);

			//if enemy has no health
			if(current->getHealth() == 0)
			{
				//if enemy is tail, set previous to tail
				if(current == entityTail)
				{
					entityTail = previous;
				}
				//if enemy is head, set next in list as head
				if(current == entityHead)
				{
					entityHead = current->next;
					continue;
				}

				//set previous node's next to current nodes next, eliminating current from the list
				previous->next = current->next;

				//free currents data
				current->free();

				//set current to the previous node
				current = previous;

				//decrement enemy count
				enemyCount--;
			}

			//set previous node to current node before iterating
			previous = current;
		}
	}
}

void Scene::spawnEnemies(SDL_Texture* enemyTexture)
{
	//spawn only if countdown done and less than 15 enemies exist
	if(--enemyCountdown <= 0 && enemyCount < ENEMY_SPAWN_LIMIT)
	{
		Sprite* enemy = new Sprite(this, false, ENTITY, enemyTexture);
		int spawnX, spawnY;

		//set spawn point to a border
		//generate number from 0 to 1, if 0 enemy will spawn on left or right border, if 1 on top or bottom border
		if(getRand() == 0)
		{
			//set spawnX to either left or right boundary
			spawnX = getRand() * (SCREEN_WIDTH - enemy->getWidth());
			//spawnY can be any y value in the height
			spawnY = rand() % (SCREEN_HEIGHT - enemy->getHeight());
		}
		else
		{
			//set spawnX to any x value in width
			spawnX = rand() % (SCREEN_WIDTH - enemy->getWidth());
			//spawnY must be on a boundary
			spawnY = getRand() * (SCREEN_HEIGHT - enemy->getHeight());
		}

		//set enemy position to spawn points
		enemy->setPos(spawnX, spawnY);

		//reset spawn timer
		//TODO change with battery
		enemyCountdown = 5 + (rand() % 25);

		//iterate the enemyCounter
		enemyCount++;
	}
}

void Scene::collisionCheck()
{
	int x = player->getX(), y = player->getY(), width = player->getWidth(), height = player->getWidth();

	x += width / 3;
	y += height / 3;
	width /= 4;
	height /= 4;

	for (Sprite* current = entityHead; current != NULL; current = current->next)
	{
		if (!current->isPlayer() && collision(x, y, width, height, current->getX(), current->getY(), current->getWidth(), current->getHeight()))
		{
			player->setHealth(0);
			player->setHealth(0);
		}
	}
}

SDL_Point Scene::getPlayerPos()
{
	//iterate through entity list for player
	for(Sprite* current = entityHead; entityHead != NULL; current = current->next)
	{
		//if player, return coords
		if(current->isPlayer())
		{
			return current->getCenter();
		}
	}

	//if player isn't found, return center of window
	return { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
}

void Scene::setPlayer(Sprite* playerSprite)
{
	player = playerSprite;
}

//TODO remove after testing
void Scene::print()
{
	int count = 0;

	for (Sprite* current = projectileHead; current != NULL; current = current->next)
	{
		count++;
	}

	//printf("Projectiles: %d\n", count);
	//printf("Enemies: %d\n", enemyCount);
	//printf("Player Health: %d\n", player->getHealth());
}
