/*
Title:	Sprite.cpp
Author:	Austin Sands
Date:	10/25/2022
Purpose: implementation file for Sprite class for my game engine
 */

#include "Sprite.h"
#include <cstdio>

//sprite constants
const int PLAYER_SPEED = 5;
const int PLAYER_HEALTH = 1;
const int PROJECTILE_SPEED = 30;
const int ENEMY_HEALTH = 1;
const int RELOAD_TIME = 8;
const int PROJECTILE_WIDTH = 7;
const int PROJECTILE_HEIGHT = 3;
const float MUZZLE_LENGTH = 54;
const float MUZZLEY_OFFSET = 16;

Sprite::Sprite()
{
	//initialize everything to NULL
	spriteScene = NULL;

	spriteRenderer = NULL;

	x = NULL;
	y = NULL;
	width = NULL;
	height = NULL;
	center = { 0, 0 };

	dX = NULL;
	dY = NULL;

	imgAngle = NULL;

	player = false;

	health = 0;

	reloading = 0;

	next = NULL;

	spriteTexture = NULL;
}

Sprite::Sprite(Scene* scene, bool player, SpriteType spriteType, SDL_Texture* texture)
{
	//assign scene
	spriteScene = scene;

	//get renderer from scene
	spriteRenderer = spriteScene->getRenderer();

	//initialize position and dimension variables
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	center = { 0, 0 };

	//initialize movement variables
	dX = 0;
	dY = 0;

	//set image angle
	imgAngle = 0;

	//set player flag
	this->player = player;

	//set reloading variable to 0
	reloading = 0;

	//set health to appropriate amount
	if (isPlayer()) { health = PLAYER_HEALTH; }
	else { health = ENEMY_HEALTH; }


	//load sprite image to texture. If no texture passed and default is used, it will print a warning to console
	setTexture(texture);

	if (spriteType == PROJECTILE) { projectile = true; }

	//initialize pointer to sprite next in list
	next = NULL;

	//add sprite to appropriate list
	spriteScene->addSprite(this, spriteType);
}

Sprite::~Sprite()
{
	free();
}

void Sprite::free()
{
	////don't want to destroy projectile and enemy textures during game
	//if(!projectile)
	//{
	//	//if there is a texture saved, destroy it
	//	if (spriteTexture != NULL)
	//	{
	//		SDL_DestroyTexture(spriteTexture);
			spriteTexture = NULL;

			//reset dimensions since no texture
			width = NULL;
			height =NULL;
	//	}
	//}

	spriteScene = NULL;

	spriteRenderer = NULL;

	spriteTexture = NULL;

	x = NULL;
	y = NULL;
	center = { NULL, NULL };

	width = NULL;
	height = NULL;

	imgAngle = NULL;

	dX = NULL;
	dY = NULL;

	player = NULL;

	health = NULL;;

	reloading = NULL;

	next = NULL;
}

void Sprite::setTexture(SDL_Texture* texture)
{
	//check if passed texture is valid
	if(texture != NULL)
	{
		spriteTexture = texture;

		//reset width and height to image dimensions
		SDL_QueryTexture(spriteTexture, NULL, NULL, &width, &height);
	}
	else
	{
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_WARN, "Texture passed to sprite is NULL\n");
	}
}

void Sprite::draw()
{
	//create rect from image dimensions
	SDL_Rect textureRect = { x, y, width, height };

	//render to screen
	SDL_RenderCopyEx(spriteRenderer, spriteTexture, NULL, &textureRect, imgAngle, NULL, SDL_FLIP_NONE);
}

void Sprite::setPos(int x, int y)
{
	//set sprite position with given input
	this->x = x;
	this->y = y;

	//set sprite center
	calcCenter();
}

void Sprite::fireProjectile()
{
	//ensure is player
	if(isPlayer())
	{
		//fire speed limit
		auto* projectile = new Sprite(spriteScene, false, PROJECTILE, spriteScene->getPlayerProjectile());

		//calculate muzzle position for projectile origin
		float muzzleX = center.x - (MUZZLEY_OFFSET * sin(getImgAngle())) + (MUZZLE_LENGTH * cos(getImgAngle()));
		float muzzleY = center.y + (MUZZLEY_OFFSET * cos(getImgAngle())) + (MUZZLE_LENGTH * sin(getImgAngle()));

		//set projectile position to originate at player center
		projectile->setPos(muzzleX, muzzleY);

		//make projectile face same direction as player image
		projectile->imgAngle = this->imgAngle;

		//set muzzle rect and render muzzle flash
		SDL_Rect muzzleTextureRect = { static_cast<int>(muzzleX - (6 * abs(sin(getImgAngle())))), static_cast<int>(muzzleY - (5 * abs(cos(getImgAngle())))), 10, 6 };
		SDL_RenderCopyEx(spriteRenderer, spriteScene->getMuzzleFlash(),NULL, &muzzleTextureRect, imgAngle, NULL, SDL_FLIP_NONE);

		//calculate dx and dy from image angle (direction facing)
		projectile->calcVector(PROJECTILE_SPEED);

		//set reloading time so can't fire for certain amount of frames
		reloading = RELOAD_TIME;
	}
}

void Sprite::doPlayer()
{
	//set center
	calcCenter();

	//calculate rotation for sprite and save in imgAngle
	calcImgAngle(center);

	//pointer to keyboard array
	int* keyboardInput = spriteScene->getKeyboard();

	//check if sprite is player
	if (isPlayer())
	{
		//set movement variables to 0 in case they're already set
		dX = 0;
		dY = 0;

		//decrements reloading like a timer
		if(reloading > 0)
		{
			reloading--;
		}
		if (keyboardInput[SDL_SCANCODE_W] || keyboardInput[SDL_SCANCODE_UP])
		{
			//negate player speed since Y = 0 is top of window
			dY = -(PLAYER_SPEED);
		}
		if(keyboardInput[SDL_SCANCODE_S] || keyboardInput[SDL_SCANCODE_DOWN])
		{
			dY = PLAYER_SPEED;
		}
		if(keyboardInput[SDL_SCANCODE_A] || keyboardInput[SDL_SCANCODE_LEFT])
		{
			dX = -(PLAYER_SPEED);
		}
		if(keyboardInput[SDL_SCANCODE_D] || keyboardInput[SDL_SCANCODE_RIGHT])
		{
			dX = PLAYER_SPEED;
		}

		//add input to position
		x += dX;
		y += dY;

		if(spriteScene->getMouseLeft() && reloading == 0)
		{
			fireProjectile();
		}
	}
}

void Sprite::moveSprite()
{
	//move projectile
	x += dX;
	y += dY;

	//calc center
	calcCenter();
}

void Sprite::moveEnemy(float speed)
{
	//find player
	calcImgAngle(center, spriteScene->getPlayerPos());

	//calculate vector to player
	calcVector(speed);

	//move enemy
	moveSprite();
}

void Sprite::calcImgAngle(SDL_Point origSpriteCenter, SDL_Point destSpriteCenter )
{
	//variables to hold components
	int xComponent = NULL;
	int yComponent = NULL;

	//we only want to change angle if is the player object
	if(isPlayer())
	{
		//variables for x and y components
		xComponent = spriteScene->getMousePos().x - origSpriteCenter.x;
		yComponent = spriteScene->getMousePos().y - origSpriteCenter.y;
	}
	else
	{
		xComponent = destSpriteCenter.x - origSpriteCenter.x;
		yComponent = destSpriteCenter.y - origSpriteCenter.y;
	}

	imgAngle = atan2(yComponent, xComponent) * 180 / M_PI;
}

void Sprite::setHealth(int newHealth)
{
	health = newHealth;
}

void Sprite::calcCenter()
{
	center = { x + (width / 2), y + (height / 2) };
}

void Sprite::calcVector(int speed)
{
	dX = speed * cos(imgAngle * M_PI / 180);
	dY = speed * sin(imgAngle * M_PI / 180);
}
