#pragma once
#include "ParticleSystem.h"
#include "GameSystem.h"

class Grid : public GameObject {
public:
	Grid() {};
	Grid(GameSystem *owner, std::string filenameGrid, std::string filenameParticle, float step, Vector2f size);
	void Update(float dt);
	void Draw();
	bool Exists();
	sf::Vector2f mousePtr;
	float radius;
	bool exists = true;
private:
	float step;
	Sprite spriteGrid;
	Sprite spriteMousePtr;
	GameSystem *owner;
};