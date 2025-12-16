#include "objRose.hpp"
#include "gameManager.hpp"
#include "texture.hpp"
#include "assetManager.hpp"
#include <stdexcept>

ObjRose::ObjRose(GameManager *gameManager, double x, double y, double goalX, double goalY, int score) : RenderableObject("ObjRose", gameManager, x, y)
{
	goal = gmtl::Vec2d(goalX, goalY);
	moveVector = gmtl::Vec2d(0.0, 0.0);
	this->score = score;

	setRoseSize(RoseSize::ROSE_SIZE_SMALL);
}

void ObjRose::update(double deltaTime)
{
	gmtl::Vec2d desiredMoveVector;
	gmtl::Vec2d distance;

	desiredMoveVector = goal - pos;
	gmtl::normalize(desiredMoveVector);
	desiredMoveVector *= ROSE_SPEED;

	gmtl::lerp(moveVector, VELOCITY_LERP_RATE, moveVector, desiredMoveVector);

	pos += moveVector * deltaTime;

	distance = goal - pos;

	if (gmtl::length(distance) <= GOAL_LOCATION_RANGE)
	{
		if (addScoreCallback == NULL)
		{
			throw std::runtime_error("ObjRose: addScoreCallback was not set!");
		}

		addScoreCallback(score);
		destroy();	
	}
}

void ObjRose::renderGui(SDL_Renderer *renderer)
{
	//Draw the rose
	roseTexture->renderCentered(renderer, pos[0], pos[1]);	
}

void ObjRose::setRoseSize(RoseSize size)
{
	std::string textureName;

	roseSize = size;

	switch (roseSize)
	{
		case RoseSize::ROSE_SIZE_SMALL:
		{
			textureName = "rose_small";
		}
		break;

		case RoseSize::ROSE_SIZE_MEDIUM:
		{
			textureName = "rose_medium";
		}
		break;

		case RoseSize::ROSE_SIZE_LARGE:
		{
			textureName = "rose";
		}
		break;
	}

	roseTexture = gameManager->getAssetManager()->getTexture(textureName);

	if (roseTexture == NULL)
	{
		SDL_Log("ObjRose: Failed to load texture: %s", textureName.c_str());
		throw std::runtime_error("ObjRose: Failed to load rose texture.");
	}
}

void ObjRose::setAddScoreCallback(std::function<void(int)> callback)
{
	addScoreCallback = std::move(callback);
}
