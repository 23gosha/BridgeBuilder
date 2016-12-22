#include "Car.h"
#include "Sprite.h"
#include "Plank.h"
//#include "ParticleSystem.h"


Car::Car(GameSystem *owner, Vector2f pos, Vector2f carSize) {
	this->owner = owner;
	sprite = Sprite("data/Car.png");
	Vector2f tmpVector = carSize;
	this->particles[0] = this->owner->GetParticleSystem()->AddParticle(pos + tmpVector * 0.5f, 1.0f);
	tmpVector = Vector2f(-carSize.x, carSize.y);
	this->particles[1] = this->owner->GetParticleSystem()->AddParticle(pos + tmpVector * 0.5f, 1.0f);
	tmpVector = (-1) * carSize;
	this->particles[2] = this->owner->GetParticleSystem()->AddParticle(pos + tmpVector * 0.5f, 1.0f);
	tmpVector = Vector2f(carSize.x, -carSize.y);
	this->particles[3] = this->owner->GetParticleSystem()->AddParticle(pos + tmpVector * 0.5f, 1.0f);
	int count = 4;
	ParticleSystem * sys = this->owner->GetParticleSystem();
	for (int i = 0; i < count; i++)
	{
		links[i] = this->owner->GetParticleSystem()->AddLink(particles[i], particles[(i + 1) % count], 0.05f);
	}
	links[4] = this->owner->GetParticleSystem()->AddLink(particles[0], particles[2], 0.05f);
	v = 5.0f;
}

Car::~Car()
{
	for (int i = 0; i < 5; i++)
		owner->GetParticleSystem()->DeleteLink(links[i]);
}

void Car::Update(float dt) {
	for (int i = 0; i < 4; i++)
	{
		/*if (particles[i]->pos.y > 800.0f)
		{
			for (int i = 0; i < 4; i++)
				particles[i]->isFixed = true;

			// SOUND
			if (!soundPlayed) {
				if (buffer.loadFromFile("data/sounds/1.wav"))
					std::cout << "Sound loaded successfully\n";
				else
					std::cout << "Error with sound";

				snd.setBuffer(buffer);
				//snd.play();
				soundPlayed = true;
			}
		}*/
		if (owner->testMode)
		{
			particles[i]->Integrate(dt);
			links[i]->Solve();
		}

	}
	links[4]->Solve();
	Vector2f alpha = Vector2f(10.0f, 0.0f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && (v<45.0f))
		v += 0.1f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && (v>-45.0f))
		v -= 0.1f;

	for (int i = 0; i < 4; i++)
		particles[i]->acceleration = particles[i]->acceleration + alpha * (v - (particles[0]->pos.x - particles[0]->prevPos.x) / dt);
	//std::cout << (v - (particles[0]->pos.x - particles[0]->prevPos.x) / dt) << "\n";
}

void Car::Draw() {
	Vector2f relativeCentre = (particles[0]->pos - particles[2]->pos) * 0.5f;
	Vector2f size = Vector2f((particles[0]->pos - particles[1]->pos).Length() * 0.5f, (particles[2]->pos - particles[1]->pos).Length() * 0.5f);
	Vector2f pos = particles[2]->pos + relativeCentre; // +size;
	float ang = std::atan2f((particles[0]->pos - particles[1]->pos).y, (particles[0]->pos - particles[1]->pos).x);
	sprite.DrawWorldSpace(owner->GetWindow(), pos, ang, size, owner->cam, sf::Color::White);
}

bool Car::Exists() {
	return exists;
}