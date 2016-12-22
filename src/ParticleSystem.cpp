#include "ParticleSystem.h"

SceneSize::SceneSize(size_t startIndex, size_t count)
{
	this->startIndex = startIndex;
	this->count = count;
}

void Particle::Integrate(float dt)
{
	if (!isFixed) {
		Vector2f tmp = pos;
		pos = pos + (pos - prevPos) + acceleration * dt * dt;
		prevPos = tmp;

		acceleration = Vector2f(0.0f, 1.8f);
		acceleration = acceleration + (prevPos - pos) * 1e1f;
		//std::cout << acceleration.Length() << "\n";
	}
}

Link::Link(Particle *p0, Particle *p1, float stiffness)
{
	this->p0 = p0;
	this->p1 = p1;
	this->stiffness = stiffness;
	this->defLength = (p1->pos - p0->pos).Length();
	this->isPhysical = true;
}


void Link::Solve()
{
	if ((p0 == NULL) || (p1 == NULL))
		return;
	Vector2f delta = p1->pos - p0->pos;
	float deform = delta.Length() - this->defLength;

	/*if (deform > 4.0f)
	{
	this->p0 = NULL;
	this->p1 = NULL;
	return;
	}*/


	if (!(p0->isFixed) && !(p1->isFixed))
	{
		p0->pos = p0->pos + delta.GetNorm() * deform * 0.5 * stiffness;
		p1->pos = p1->pos + delta.GetNorm() * deform * -0.5 * stiffness;
	}
	else if ((p0->isFixed) && !(p1->isFixed))
	{
		p1->pos = p1->pos - delta.GetNorm() * deform * stiffness;
	}
	else if ((p1->isFixed) && !(p0->isFixed))
	{
		p0->pos = p0->pos + delta.GetNorm() * deform * stiffness;
	}
}

VolumeLink::VolumeLink(Particle **particles, int particleCount, float initialPressure, float mainPressure)
{
	for (int i = 0; i < particleCount; i++) {
		(this->particles).push_back(particles[i]);
	}
	this->initialPressure = initialPressure;
	this->mainPressure = mainPressure;
	initialVolume = ComputeVolume();
}

float VolumeLink::ComputeVolume()
{
	float volume = 0;
	for (size_t i = 0; i < particles.size(); i++)
	{
		volume += particles[(i + 1) % particles.size()]->pos * particles[i]->pos * 0.5f;
	}
	return volume;
}

void VolumeLink::Solve()
{
	float currVolume = ComputeVolume();
	float currPressure = initialPressure * initialVolume / currVolume;

	for (size_t i = 0; i < particles.size(); i++)
	{
		Vector2f delta = particles[(i + 1) % particles.size()]->pos - particles[i]->pos;
		Vector2f force = Vector2f(delta.y, -delta.x) * (currPressure - mainPressure);
		particles[i]->pos = particles[i]->pos + force;
		particles[(i + 1) % particles.size()]->pos = particles[(i + 1) % particles.size()]->pos + force;
	}
}

ParticleSystem::ParticleSystem(Vector2f minPoint, Vector2f maxPoint)
{
	this->minPoint = minPoint;
	this->maxPoint = maxPoint;
}

ParticleSystem::~ParticleSystem()
{
	for (size_t i = 0; i < particles.size(); i++)
		delete particles[i];
}

void ParticleSystem::Push(Vector2f way, int sceneNumber)
{
	for (size_t i = objectSizes[sceneNumber].startIndex; i < objectSizes[sceneNumber].startIndex + objectSizes[sceneNumber].count; i++) {
		particles[i]->pos.x += way.x;
		particles[i]->pos.y += way.y;
	}
}

void ParticleSystem::Update()
{
	for (size_t i = 0; i < particles.size(); i++)
	{
		particles[i]->Integrate(1e-2f);

		if (particles[i]->pos.y + particles[i]->radius>maxPoint.y)
		{
			particles[i]->pos.y = maxPoint.y - particles[i]->radius;
			particles[i]->prevPos.x = particles[i]->pos.x;
		}
		if (particles[i]->pos.y - particles[i]->radius<minPoint.y)
		{
			particles[i]->pos.y = minPoint.y + particles[i]->radius;
			particles[i]->prevPos.x = particles[i]->pos.x;
		}
		if (particles[i]->pos.x + particles[i]->radius>maxPoint.x)
		{
			particles[i]->pos.x = maxPoint.x - particles[i]->radius;
			particles[i]->prevPos.y = particles[i]->pos.y;
		}
		if (particles[i]->pos.x - particles[i]->radius<minPoint.x)
		{
			particles[i]->pos.x = minPoint.x + particles[i]->radius;
			particles[i]->prevPos.y = particles[i]->pos.y;
		}
	}

	//interactions between objects
	for (size_t k = 0; k < objectSizes.size(); k++)
	{
		int start = objectSizes[k].startIndex;
		int count = objectSizes[k].count;
		for (size_t i = start; i < start + count; i++)
			for (size_t j = start + count; j < particles.size(); j++)
			{
				Vector2f delta = particles[i]->pos - particles[j]->pos;
				float del = particles[i]->radius + particles[j]->radius - delta.Length();
				if (del > 0) {
					particles[i]->pos = particles[i]->pos + delta.GetNorm() * del * 0.5f;
					particles[j]->pos = particles[j]->pos - delta.GetNorm() * del * 0.5f;
					particles[i]->prevPos = particles[i]->pos;
					particles[j]->prevPos = particles[j]->pos;
				}
			}
	}

	for (size_t i = 0; i < links.size(); i++)
	{
		links[i]->Solve();
	}
	for (size_t i = 0; i < volumeLinks.size(); i++)
	{
		volumeLinks[i].Solve();
	}
	//SolvePhysicalLinks(dt);
	//std::cout << "SolvePhysicalLinks was called\n";
}

size_t ParticleSystem::GetParticlesCount()
{
	return particles.size();
}

Particle *ParticleSystem::AddParticle(Vector2f pos, float radius)
{
	Particle *newbie = new Particle;
	newbie->pos = pos;
	newbie->prevPos = pos;
	//newbie->acceleration = Vector2f(0.0f, 9.8f);
	//newbie->acceleration = Vector2f(0.0f, 1.8f);
	newbie->radius = radius;
	particles.push_back(newbie);

	return particles.back();
}

Link* ParticleSystem::AddLink(Particle *p0, Particle *p1, float stiffness)
{
	Link *newLink = new Link(p0, p1, stiffness);
	links.push_back(newLink);
	return links.back();
}

void ParticleSystem::AddVolumeLink(Particle **particles, int particleCount, float initialPressure, float mainPressure)
{
	volumeLinks.push_back(VolumeLink(particles, particleCount, initialPressure, mainPressure));
}

size_t ParticleSystem::WriteObjectSize(int count)
{
	objectSizes.push_back(SceneSize(GetParticlesCount(), count));
	return objectSizes.size() - 1;
}

size_t ParticleSystem::GetObjectNumber()
{
	return objectSizes.size();
}

Particle* ParticleSystem::GetParticle(int i)
{
	return particles[i];
}

Vector2f ParticleSystem::GerMinPoint()
{
	return minPoint;
}

Vector2f ParticleSystem::GerMaxPoint()
{
	return maxPoint;
}

void ParticleSystem::FixParticle(int particleIndex)
{
	particles[particleIndex]->isFixed = 1;
}

void ParticleSystem::UnFixParticle(int particleIndex)
{
	particles[particleIndex]->isFixed = 0;
}

bool ParticleSystem::isParticleFixed(int particleIndex)
{
	if (particles[particleIndex]->isFixed == 1)
		return true;
	else
		return false;
}

size_t ParticleSystem::GetParticleCount()
{
	return particles.size();
}


int ParticleSystem::ParticleIndex(Vector2f pos)
{
	for (int i = 0; i < this->GetParticleCount(); i++)
	{
		if ((particles[i]->pos - pos).Length() < 0.01)
			return i;
	}
	return -1;
}

int ParticleSystem::LinkIndex(Link* link)
{
	for (int i = 0; i < links.size(); i++)
	{
		if (link == links[i])
			return i;
	}
	return -1;
}

bool ParticleSystem::AreLinksCrossing(Link link0, Link link1)
{
	Vector2f v00 = link0.p0->pos - link1.p0->pos;
	Vector2f v01 = link0.p1->pos - link1.p0->pos;
	Vector2f v10 = link0.p0->pos - link1.p1->pos;
	Vector2f v11 = link0.p1->pos - link1.p1->pos;
	if ((v00 ^ v01) * (v10 ^ v11) > -0.0000001)
	{
		return false;
	}
	v00 = link1.p0->pos - link0.p0->pos;
	v01 = link1.p1->pos - link0.p0->pos;
	v10 = link1.p0->pos - link0.p1->pos;
	v11 = link1.p1->pos - link0.p1->pos;
	if ((v00 ^ v01) * (v10 ^ v11) > -0.0000001)
	{
		return false;
	}
	return true;
}

bool ParticleSystem::AreLinkAndSegmentCrossing(Link link, Vector2f pos0, Vector2f pos1)
{
	Vector2f v00 = link.p0->pos - pos0;
	Vector2f v01 = link.p1->pos - pos0;
	Vector2f v10 = link.p0->pos - pos1;
	Vector2f v11 = link.p1->pos - pos1;
	if ((v00 ^ v01) * (v10 ^ v11) > -0.0000001)
	{
		return false;
	}
	return true;
}

void ParticleSystem::SolvePhysicalLinks(float dt)
{
	for (int firstLinkIndex = 0; firstLinkIndex < links.size(); firstLinkIndex++)
		for (int secondLinkIndex = firstLinkIndex + 1; secondLinkIndex < links.size(); secondLinkIndex++)
		{
			Link *link0 = links[firstLinkIndex];
			Link *link1 = links[secondLinkIndex];
			if ((link0->isPhysical) && (link1->isPhysical) && AreLinksCrossing(*link0, *link1))
			{
				//std::cout << "Links are crossing\n";
				SolveTwoLinks(link0, link1, dt);
			}
			//std::cout << "Links are not crossing\n";
		}
}

void ParticleSystem::SolveTwoLinks(Link *link0, Link *link1, float dt)
{
	Particle *particle = link0->p0;
	Link *link = link1;
	if (ParticleLinkDistanse(*link0->p1, *link1) < ParticleLinkDistanse(*particle, *link))
	{
		particle = link0->p1;
		link = link1;
	}
	if (ParticleLinkDistanse(*link1->p0, *link0) < ParticleLinkDistanse(*particle, *link))
	{
		particle = link1->p0;
		link = link0;
	}
	if (ParticleLinkDistanse(*link1->p1, *link0) < ParticleLinkDistanse(*particle, *link))
	{
		particle = link1->p1;
		link = link0;
	}

	float m = 1.0f; // Масса частицы
	float M = 2.0f; // Масса балки
	float k = 50.0f; // Коэффициент 100
					  //Vector2f crossPoint = CrossPoint(*link0, *link1);
	Vector2f crossPoint = PullPoint(*particle, *link);
	Vector2f r0 = link->p0->pos - crossPoint;
	Vector2f r1 = link->p1->pos - crossPoint;
	//float J = m * (r0 * r0 + r1 * r1);    // Момент инерции
	float J = m * ((r0.Length() + r1.Length()) * (r0.Length() + r1.Length() / 2));    // Момент инерции
	float delta = ParticleLinkDistanse(*particle, *link);
	Vector2f DELTA = (link->p1->pos - link->p0->pos).GetPerp();
	if (!AreLinkAndSegmentCrossing(*link, particle->pos, particle->pos + 2 * DELTA))
		DELTA = DELTA * (-1);
	float a = delta * k * m / M;
	float a0 = a + (m * k) / (M * J) * ((DELTA ^ r0) ^ r0);
	float a1 = a + (m * k) / (M * J) * ((DELTA ^ r1) ^ r1);

	float delta0 = a0 * dt * dt;
	float delta1 = a1 * dt * dt;
	Vector2f direction = DELTA.GetNorm();
	//link->p0->prevPos = link->p0->pos; // -delta0 * direction;
	//link->p1->prevPos = link->p1->pos; // -delta1 * direction;
	if (!link->p0->isFixed) link->p0->pos = link->p0->pos - delta0 * direction; // -
	if (!link->p1->isFixed)link->p1->pos = link->p1->pos - delta1 * direction; // -
																			   //link->p0->prevPos = link->p0->pos; // -delta0 * direction;
																			   //link->p1->prevPos = link->p1->pos; // -delta1 * direction;
	if (!particle->isFixed) particle->pos = particle->pos + direction * delta * k * dt * dt;
	//particle->prevPos = particle->pos;
}

float ParticleSystem::ParticleLinkDistanse(Particle particle, Link link)
{
	float a = (particle.pos - link.p0->pos).Length();
	float b = (particle.pos - link.p1->pos).Length();
	float c = (link.p0->pos - link.p1->pos).Length();
	float p = (a + b + c) / 2.0f;
	float square = sqrt(fabs(p * (p - a) * (p - b) * (p - c)));
	float h = square / c;
	return h;
}

Vector2f ParticleSystem::CrossPoint(Link link0, Link link1)
{
	Vector2f AB = link0.p1->pos - link0.p0->pos;
	Vector2f AC = link1.p0->pos - link0.p0->pos;
	Vector2f AD = link1.p1->pos - link0.p0->pos;
	float z1 = AB ^ AC;
	float z2 = AB ^ AD;
	float x = link1.p0->pos.x + (link1.p1->pos.x - link1.p0->pos.x) * fabs(z1) / fabs(z2 - z1);
	float y = link1.p0->pos.y + (link1.p1->pos.y - link1.p0->pos.y) * fabs(z1) / fabs(z2 - z1);
	return Vector2f(x, y);
}

Vector2f ParticleSystem::PullPoint(Particle particle, Link link)
{
	Vector2f perpendicular = (link.p1->pos - link.p0->pos).GetPerp().GetNorm();
	Vector2f point = particle.pos + perpendicular * ParticleLinkDistanse(particle, link);
	if ((link.p0->pos - point) * (point - link.p1->pos) > 0.01)
		point = particle.pos - perpendicular * ParticleLinkDistanse(particle, link);
	return point;
}

int ParticleSystem::DeleteLink(Link* link)
{
	if (LinkIndex(link))
	{
		links[LinkIndex(link)] = links.back();
		links.pop_back();
		return 0;
	}
	else
		return -1;
}