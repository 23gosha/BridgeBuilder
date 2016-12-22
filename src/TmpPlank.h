#pragma once
#include "ParticleSystem.h"
#include "GameSystem.h"
#include "Sprite.h"

class TmpPlank : public GameObject {
public:
	TmpPlank() {};
	TmpPlank(GameSystem *owner, Vector2f pos0, Vector2f pos1);
	void Update(float dt) {};
	void Draw();
	bool Exists();
	bool isBroken = false;
	Vector2f pos0;
	Vector2f pos1;
	bool exists = true;
private:
	float width;
	Sprite sprite;
	GameSystem *owner;
};