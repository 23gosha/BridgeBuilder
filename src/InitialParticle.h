#pragma once
#include "ParticleSystem.h"
#include "GameSystem.h"
#include "Sprite.h"

class InitialParticle : public GameObject {
public:
	InitialParticle() {};
	InitialParticle(GameSystem *owner, Vector2f pos);
	void Update(float dt) {};
	void Draw();
	bool Exists();
	bool exists = true;
	Particle* particle;
	Sprite sprite;
private:
	GameSystem *owner;
};

void SolveTwoPlanks(Plank *plank0, Plank *plank1);