#pragma once
#include "GameSystem.h"
#include "Sprite.h"
#include <math.h>
#include "SFML/Audio.hpp"

//#include "ParticleSystem.h"
class Particle; //от кольцевых зависимостей

class Car : public GameObject { // why public??
public:
	Car() {};
	~Car();
	Car(GameSystem *owner, Vector2f pos, Vector2f size);
	void Update(float dt);
	void Draw();
	bool Exists();
	bool exists = true;
private:
	Sprite sprite;
	Particle * particles[4];
	Link * links[5];
	GameSystem *owner;
	bool soundPlayed = false;
	sf::SoundBuffer buffer;
	sf::Sound snd;
	float v;
};