#pragma once
#include "Plank.h"
#include "ParticleSystem.h"

Plank::Plank(GameSystem *owner, Vector2f pos0, Vector2f pos1)
{
	this->owner = owner;
	width = 10.0f;
	Particle* particle0 = NULL;
	if(this->owner->GetParticleSystem()->ParticleIndex(pos0) != -1)
		particle0 = this->owner->GetParticleSystem()->GetParticle(this->owner->GetParticleSystem()->ParticleIndex(pos0));
	else
		particle0 = this->owner->GetParticleSystem()->AddParticle(pos0, 1.0f);

	Particle* particle1 = NULL;
	if(this->owner->GetParticleSystem()->ParticleIndex(pos1) == -1)
		particle1 = this->owner->GetParticleSystem()->AddParticle(pos1, 1.0f);
	else
		particle1 = this->owner->GetParticleSystem()->GetParticle(this->owner->GetParticleSystem()->ParticleIndex(pos1));
	link = owner->GetParticleSystem()->AddLink(particle0, particle1, 0.003f);
	//link = new Link(particle0, particle1, 0.05f);
	sprite = Sprite("data/woodLink.png");
}

Plank::~Plank()
{
	owner->GetParticleSystem()->DeleteLink(link);
}

void Plank::Update(float dt)
{
	if (owner->testMode == false)
	{
		return;
	}
	link->Solve();
	if (link->p0->pos.y > 800.0f)
		link->p0->isFixed = true;
	if (link->p1->pos.y > 800.0f)
		link->p1->isFixed = true;

	link->p0->Integrate(dt);
	link->p1->Integrate(dt);

	Vector2f delta = link->p1->pos - link->p0->pos;
	float deform = delta.Length() - link->defLength;
	if (deform > criticalDeform)
	{
		Particle* particle0 = owner->GetParticleSystem()->AddParticle(link->p0->pos, 1.0f);
		link->p0 = particle0;
	}
}

void Plank::Draw() 
{
	//sprite.LinkDrawWorldSpace(this->owner->GetWindow(), link.p0->pos, link.p1->pos, 20.0f, owner->cam);
	Vector2f delta = link->p1->pos - link->p0->pos;
	//float width = 10.0f;
	float deform = delta.Length() - link->defLength;
	float nonRedValue = (criticalDeform - abs(deform)) * 255 / criticalDeform;
	sf::Color color = sf::Color::Color(255, nonRedValue, nonRedValue, 255);
	if(!(link->isPhysical))
		color = sf::Color::Color(100, nonRedValue, nonRedValue, 255);
	sprite.DrawWorldSpace(this->owner->GetWindow(), link->p0->pos + delta * 0.5f, atan2(delta.y, delta.x), Vector2f(delta.Length() / 2.0f, width), owner->cam, color);

}

Link Plank::GetLink()
{
	return *link;
}

bool Plank::AreCrossing(Plank plank)
{
	Vector2f v00 = plank.GetLink().p0->pos - link->p0->pos;
	Vector2f v01 = plank.GetLink().p1->pos - link->p0->pos;
	Vector2f v10 = plank.GetLink().p0->pos - link->p1->pos;
	Vector2f v11 = plank.GetLink().p1->pos - link->p1->pos;
	if ((v00 ^ v01) * (v10 ^ v11) > 0)
	{
		return false;	
	}
	v00 = link->p0->pos - plank.GetLink().p0->pos;
	v01 = link->p1->pos - plank.GetLink().p0->pos;
	v10 = link->p0->pos - plank.GetLink().p1->pos;
	v11 = link->p1->pos - plank.GetLink().p1->pos;
	if ((v00 ^ v01) * (v10 ^ v11) > 0)
	{
		return false;
	}
	return true;
}

void SolveTwoPlanks(Plank *plank0, Plank *plank1)
{
	//std::cout << plank0->AreCrossing(*plank1) << '\n';
}

bool Plank::Exists()
{
	return exists;
}

float Plank::GetWidth()
{
	return width;
}