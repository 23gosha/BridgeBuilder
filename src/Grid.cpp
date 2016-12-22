#include "Grid.h"

Grid::Grid(GameSystem *owner, std::string filenameGrid, std::string filenameParticle, float step, Vector2f size) {
	this->owner = owner;
	spriteGrid = Sprite(filenameGrid);
	this->step = step;
	mousePtr = sf::Vector2f(0.0f, 0.0f);
	radius = 5.0f;
	spriteGrid.tex->setRepeated(true); // !!! doesn't work
	spriteMousePtr = Sprite("data/zazaka.png");

	//owner->GetParticleSystem()->FixParticle(owner->GetParticleSystem()->ParticleIndex(p0->pos));
	//owner->GetParticleSystem()->FixParticle(owner->GetParticleSystem()->ParticleIndex(p1->pos));
	//this->sprite.tex->setRepeated(true);
	//owner->grid = this;
}

void Grid::Draw() {
	if(!owner->testMode)
		this->spriteGrid.DrawWorldSpace(owner->GetWindow(), Vector2f(owner->GetWindow()->getSize().x / 2.0f, owner->GetWindow()->getSize().y / 2.0f), 0.0f, Vector2f(step*24.0f / 2.0f, step*24.0f / 2.0f), owner->cam, sf::Color::White);
	this->spriteMousePtr.DrawWorldSpace(owner->GetWindow(), Vector2f(mousePtr.x, mousePtr.y), 0.0f, Vector2f(20.0f, 20.0f), owner->cam, sf::Color::White);
	//sf::CircleShape circle;
	//circle.setFillColor(sf::Color::Green);
	//circle.setOrigin(position);
	//circle.setPosition(mousePtr - sf::Vector2f(radius, radius));
	//circle.setRadius(radius);
	//owner->GetWindow()->draw(circle);
}
void Grid::Update(float dt) {
	sf::RenderWindow * wnd = (owner->GetWindow());

	Vector2f posScreenCoord = Vector2f(owner->cam.fieldOfView * sf::Mouse::getPosition(*wnd).x / wnd->getSize().x,
		owner->cam.fieldOfView * sf::Mouse::getPosition(*wnd).y / wnd->getSize().x);
	Vector2f xVector = Vector2f(owner->cam.ang);
	Vector2f yVector = Vector2f(owner->cam.ang + 3.1415926 / 2.0f);
	Vector2f posScreenZero = owner->cam.pos - (xVector * owner->cam.fieldOfView) * 0.5f -
		(yVector * owner->cam.fieldOfView * (owner->GetWindow()->getSize().y / owner->GetWindow()->getSize().x)) * 0.5f;
	Vector2f position0 = posScreenZero + posScreenCoord;
	//std::cout << position0.x << "\n";

	sf::Vector2f position = sf::Vector2f(position0.x, position0.y);
	if (fabs(position.x - floor(position.x / step) * step) < step / 2)
		mousePtr.x = floor(position.x / step) * step;
	else
		mousePtr.x = floor(position.x / step) * step + step;
	if (fabs(position.y - floor(position.y / step) * step) < step / 2)
		mousePtr.y = floor(position.y / step) * step;
	else
		mousePtr.y = floor(position.y / step) * step + step;
}

bool Grid::Exists() {
	return exists;
}
