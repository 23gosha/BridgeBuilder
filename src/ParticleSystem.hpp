#include "Vector2f"
#include <vector>
#include <math.h>

class Particle
{
public:
	void Integrate(float dt)
	{
		if (!isFixed)
		{
			Vector2f currPos = pos;
			pos = pos + (pos - prevPos) + acceleration * dt * dt;
			prevPos = currPos;
		}
	}
	int isFixed = 0;
	float radius;
	Vector2f pos;
	Vector2f prevPos;
	Vector2f acceleration;
};

class Link
{
public:
	Link()
	{

	}
	Link(Particle *p0, Particle *p1, float stiffness, float mult)
	{
		this->p0 = p0;
		this->p1 = p1;
		this->stiffness = stiffness;
		this->defLength = mult * (p0->pos - p1->pos).Length();
	}
	void Solve()
	{
		if ((p0 == NULL) || (p1 == NULL))
			return;
		Vector2f delta = p1->pos - p0->pos;
		float deform = delta.Length() - this->defLength;

		if (deform > 4.0f)
		{
			this->p0 = NULL;
			this->p1 = NULL;
			return;
		}

			
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
	Particle *p0, *p1;
	float stiffness;
	float defLength;
};

class VolLink
{
public:
	VolLink()
	{

	}
	VolLink(Particle** particles, int particleCount, float stiffness)
	{
		this->stiffness = stiffness;
		this->particleCount = particleCount;
		for (int i = 0; i < particleCount; i++)
			this->volumeParticles.push_back(particles[i]);
		initalVolume = GetVolume();
		initalPressure = 0.1f;
	}
	float GetVolume()
	{
		float volume = 0;
		for (int i = 1; i < particleCount - 1; i++)
			volume += (volumeParticles[i]->pos - volumeParticles[0]->pos) ^ (volumeParticles[i + 1]->pos - volumeParticles[0]->pos);
		return volume;
	}
	void Solve()
	{
		//float deltaArea = this->GetArea() - defArea;
		//float pressure = stiffness * defArea / this->GetVolume();
		float currVolume = GetVolume();
		float pressureDiff = initalPressure * (initalVolume / currVolume - 1.0f);
		for (int i = 0; i < volumeParticles.size(); i++)
		{
			Vector2f delta = volumeParticles[(i + 1) % volumeParticles.size()]->pos - volumeParticles[i]->pos;
			Vector2f force = pressureDiff * Vector2f(delta.y, -delta.x);
			volumeParticles[i]->pos = volumeParticles[i]->pos + force;
			volumeParticles[(i + 1) % volumeParticles.size()]->pos = volumeParticles[(i + 1) % volumeParticles.size()]->pos + force;
		}
	}
	int particleCount;
	float stiffness; // initalPressure
	float initalVolume;
	float initalPressure;
	std::vector<Particle*> volumeParticles;

};

class ParticleSystem
{
public:
	ParticleSystem()
	{
		
	}

	Particle* AddParticle(Vector2f pos, float radius)
	{
		Particle *newbie = new Particle;
		newbie->pos = pos;
		newbie->prevPos = newbie->pos;
		newbie->acceleration = Vector2f(0.0f, 10.0f);
		newbie->radius = radius;
		particles.push_back(newbie);
		return particles.back();
	}

	void AddLink(Particle *p0, Particle *p1, float stiffness, float mult)
	{
		links.push_back(Link(p0, p1, stiffness, mult));
	}
	void DeleteLink(int linkIndex)
	{
		links[linkIndex].p0 = NULL;
		links[linkIndex].p1 = NULL;
	}
	void AddVolumeLink(Particle **particles, int particleCount, float stiffness)
	{
		volLinks.push_back(VolLink(particles, particleCount, stiffness));
	}
	void FixParticle(int particleIndex)
	{
		particles[particleIndex]->isFixed = 1;
	}
	void UnFixParticle(int particleIndex)
	{
		particles[particleIndex]->isFixed = 0;
	}
	void Update()
	{
		for (size_t particleIndex = 0;
			particleIndex < particles.size(); particleIndex++)
		{
			particles[particleIndex]->Integrate(1e-2f);
		
			float floorLevel = 800.0f;
			float wall = 1200;
			if (particles[particleIndex]->pos.y + particles[particleIndex]->radius > floorLevel)
			{
				particles[particleIndex]->pos.y = floorLevel - particles[particleIndex]->radius;
				particles[particleIndex]->prevPos.x = particles[particleIndex]->pos.x;
			}
			if (particles[particleIndex]->pos.y - particles[particleIndex]->radius < 0)
			{
				particles[particleIndex]->pos.y = particles[particleIndex]->radius;
				particles[particleIndex]->prevPos.x = particles[particleIndex]->pos.x;
			}
			if (particles[particleIndex]->pos.x - particles[particleIndex]->radius < 0)
			{
				particles[particleIndex]->pos.x = particles[particleIndex]->radius;
				particles[particleIndex]->prevPos.y = particles[particleIndex]->pos.y;
			}
			if (particles[particleIndex]->pos.x + particles[particleIndex]->radius> wall)
			{
				particles[particleIndex]->pos.x = wall - particles[particleIndex]->radius;
				particles[particleIndex]->prevPos.y = particles[particleIndex]->pos.y;
			}

		}
		for (size_t linkIndex = 0;
			linkIndex < links.size(); linkIndex++)
		{
			links[linkIndex].Solve();
		}
		for (size_t volLinkIndex = 0;
		volLinkIndex < volLinks.size(); volLinkIndex++)
		{
			volLinks[volLinkIndex].Solve();
		}
	}
	void Right()
	{
		particles[0]->pos = particles[0]->pos + Vector2f(step, 0.0f);
	}
	void Left()
	{
		particles[0]->pos = particles[0]->pos + Vector2f(-step, 0.0f);
	}
	void Up()
	{
		particles[0]->pos = particles[0]->pos + Vector2f(0.0f, -step);
	}
	void Down()
	{
		particles[0]->pos = particles[0]->pos + Vector2f(0.0f, step);
	}
	bool isParticleFixed(int particleIndex)
	{
		if (particles[particleIndex]->isFixed == 1)
			return true;
		else
			return false;
	}
	Particle * GetParticle(int particleIndex)
	{
		return particles[particleIndex];
	}
	size_t GetParticleCount()
	{
		return particles.size();
	}
	size_t GetLinkCount()
	{
		return links.size();
	}
	Link GetLink(int linkIndex)
	{
		return links[linkIndex];
	}
	int ParticleIndex(Vector2f pos)
	{
		for (int i = 0; i < this->GetParticleCount(); i++)
		{
			if ((particles[i]->pos - pos).Length() < 0.01)
				return i;
		}
		return -1;
	}
private:
	float step = 0.05f;
	std::vector<Particle*> particles;
	std::vector<Link> links;
	std::vector<VolLink> volLinks;
};


