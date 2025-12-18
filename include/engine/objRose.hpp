#ifndef OBJ_ROSE_H
#define OBJ_ROSE_H

#include "renderableObject.hpp"
#include <functional>
#include <gmtl/gmtl.h>
#include <gmtl/Matrix.h>

class GameManager;
class Texture;

enum RoseSize
{
	ROSE_SIZE_SMALL,
	ROSE_SIZE_MEDIUM,
	ROSE_SIZE_LARGE
};

class ObjRose : public RenderableObject
{
	public:
		ObjRose(GameManager *gameManager, double x, double y, double goalX, double goalY, int score);

		void update(double deltaTime) override;
		void renderGui(SDL_Renderer *renderer) override;

		void setRoseSize(RoseSize size);
		void setAddScoreCallback(std::function<void(int)> callback);

	private:
		const int ROSE_SPEED = 416;
		static const constexpr double VELOCITY_LERP_RATE = 0.05;
		static const constexpr double GOAL_LOCATION_RANGE = 16;	//Distance to goal location needed before being collected

		Texture *roseTexture = NULL;
		RoseSize roseSize;
		gmtl::Vec2d goal;
		gmtl::Vec2d moveVector;
		int score;

		std::function<void(int)> addScoreCallback = NULL;
};

#endif
