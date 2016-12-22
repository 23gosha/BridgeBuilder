#pragma once
#include "TmpPlank.h"
#include "ParticleSystem.h"

TmpPlank::TmpPlank(GameSystem *owner, Vector2f pos0, Vector2f pos1)
{
	this->owner = owner;
	width = 10.0f;
	this->pos0 = pos0;
	this->pos1 = pos1;

	sprite = Sprite("data/woodLink.png");
}

void TmpPlank::Draw() 
{
	if (owner->linkAddingActive)
	{
		Vector2f delta = pos1 - pos0;
		sprite.DrawWorldSpace(this->owner->GetWindow(), pos0 + delta * 0.5f, atan2(delta.y, delta.x), Vector2f(delta.Length() / 2.0f, width), owner->cam, sf::Color::Green);
	}
}


bool TmpPlank::Exists() {
	return exists;
}