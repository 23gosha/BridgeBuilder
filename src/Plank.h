#pragma once
#include "ParticleSystem.h"
#include "GameSystem.h"
#include "Sprite.h"

class Plank : public GameObject {
public:
	Plank() {};
	~Plank();
	Plank(GameSystem *owner, Vector2f pos0, Vector2f pos1);
	void Update(float dt);
	void Draw();
	bool Exists();
	bool isBroken = false;
	bool AreCrossing(Plank plank);
	Link GetLink();
	bool exists = true;
	Link *link;
	float GetWidth();
private:
	float width;
	Sprite sprite;
	GameSystem *owner;
	float criticalDeform = 3.0f;
	
};

void SolveTwoPlanks(Plank *plank0, Plank *plank1);