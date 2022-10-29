/*
Title:	Sprite.h
Author:	Austin Sands
Date:	10/25/2022
Purpose: header file for Sprite class for my game engine. REQUIRES SCENE CLASS
 */

#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include <SDL.h>
#include <SDL_image.h>
#include "Scene.h"

 //sprite type enumerations
 //entity and projectiles are all that is included now, 
enum SpriteType
{
	ENTITY, PROJECTILE
};

//forward declaration
class Scene;

class Sprite
{
public:
	//default constructor
	Sprite();

	//constructor
	Sprite(Scene* scene, bool player = false, SpriteType type = ENTITY, SDL_Texture* texture = NULL);

	//destructor
	~Sprite();

	//deallocates resources
	void free();

	//void sets sprite texture
	void setTexture(SDL_Texture* texture);

	//adds sprite to renderer
	void draw();

	//sets sprite position
	void setPos(int x, int y);

	//fire a bullet
	void fireProjectile();

	//input handler for player sprite
	void doPlayer();

	//handler for projectiles
	void drawProjectiles();

	//calculate image angle
	void calcImgAngle(SDL_Point spriteCenter);

	//pointer variable for use with sprite lists
	Sprite* next;

	//getters
	int getHealth() const { return health; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	SDL_Point getCenter() const { return center; }
	bool isPlayer() const { return player; }
	double getImgAngle() const { return imgAngle * 180 / M_PI; }
	int getX() const { return x; }
	int getY() const { return y; }

private:
	//scene sprite is in
	Scene* spriteScene;

	//renderer for this sprite inherited from scene
	SDL_Renderer* spriteRenderer;

	//image texture associated with sprite
	SDL_Texture* spriteTexture;

	//gets sprite center
	void calcCenter();

	//sprite position
	int x;
	int y;
	SDL_Point center;

	//sprite dimensions
	int width;
	int height;

	//angle to display image
	double imgAngle;

	//calculate dx and dy from image angle
	void calcVector(int speed);

	//movement variables
	int dX;
	int dY;

	//flag if sprite is player sprite
	bool player;

	//health variable to tell if sprite is alive
	int health;

	//variable to ensure shooting projectile is not called every frame
	int reloading;
};
#endif
