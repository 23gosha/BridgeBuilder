#include "Background.h"

Background::Background(GameSystem *owner, std::string filename) {
	this->owner = owner;
	sprite = Sprite(filename);
	//this->sprite.tex->setRepeated(true);
}

void Background::Draw() {
	this->sprite.DrawBackground(owner->GetWindow(), 1.0f, owner->cam);
}
void Background::Update(float dt){}

bool Background::Exists() {
	return exists;
}