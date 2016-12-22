#pragma once
#include "InitialParticle.h"
#include "ParticleSystem.h"

InitialParticle::InitialParticle(GameSystem *owner, Vector2f pos)
{
	this->owner = owner;
	particle = owner->GetParticleSystem()->AddParticle(pos, 20.0f);
	particle->isFixed = true;
	sprite = Sprite("data/zazaka.png");
}

void InitialParticle::Draw()
{
	sprite.DrawWorldSpace(owner->GetWindow(), particle->pos, 0.0f, Vector2f(20.0f, 20.0f), owner->cam, sf::Color::White);
}

bool InitialParticle::Exists() {
	return exists;
}