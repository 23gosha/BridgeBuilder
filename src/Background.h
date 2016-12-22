#pragma once
#include "GameSystem.h"

//#include "ParticleSystem.h"
//class Particle; //от кольцевых зависимостей

class Background : public GameObject {
public:
	Background() {};
	Background(GameSystem *owner, std::string filename);
	void Update(float dt);
	void Draw();
	bool Exists();
	//Particle* GetParticle();
	Sprite sprite;
private:
	GameSystem *owner;
	bool exists = true;
};