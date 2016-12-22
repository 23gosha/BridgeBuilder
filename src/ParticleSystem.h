#pragma once
#include "Vector2f.h"
#include <vector>
#include <math.h>

class SceneSize
{
public:
	SceneSize(size_t startIndex, size_t count);
	size_t startIndex;
	size_t count;
};

class Particle
{
public:
	void Integrate(float dt);
	bool isFixed = 0;
	Vector2f pos;
	Vector2f prevPos;
	Vector2f acceleration; //= Vector2f(0.0f, 9.8f);
	float radius;
};

class Link
{
public:
	Link() {}
	Link(Particle *p0, Particle *p1, float stiffness);
	void Solve();
	bool isPhysical;
	Particle *p0;
	Particle *p1;
	float defLength;
	float stiffness;
};

class VolumeLink {
public:
	VolumeLink() {}
	VolumeLink(Particle **particles, int particleCount, float initialPressure, float mainPressure);
	float ComputeVolume();
	void Solve();
	std::vector<Particle*> particles;
	float defVolume, initialPressure, initialVolume, mainPressure;
};

class ParticleSystem
{
public:
	ParticleSystem(Vector2f minPoint, Vector2f maxPoint);
	~ParticleSystem();
	void Push(Vector2f way, int sceneNumber);
	void Update();
	size_t GetParticlesCount();
	Particle *AddParticle(Vector2f pos, float radius);
	Link* AddLink(Particle *p0, Particle *p1, float stiffness);
	void AddVolumeLink(Particle **particles, int particleCount, float initialPressure, float mainPressure);
	size_t WriteObjectSize(int count);
	size_t GetObjectNumber();
	Particle* GetParticle(int i);
	Vector2f GerMinPoint();
	Vector2f GerMaxPoint();
	void FixParticle(int particleIndex);
	void UnFixParticle(int particleIndex);
	bool isParticleFixed(int particleIndex);
	size_t GetParticleCount();
	int ParticleIndex(Vector2f pos);
	int LinkIndex(Link* link);
	bool AreLinksCrossing(Link link0, Link link1);
	bool AreLinkAndSegmentCrossing(Link link, Vector2f pos0, Vector2f pos1);
	void SolvePhysicalLinks(float dt);
	void SolveTwoLinks(Link *link0, Link *link1, float dt);
	float ParticleLinkDistanse(Particle particle, Link link);
	Vector2f CrossPoint(Link link0, Link link1);
	Vector2f PullPoint(Particle particle, Link link);
	int DeleteLink(Link* link);
private:
	std::vector<Particle*> particles;
	std::vector<Link *> links;
	std::vector<VolumeLink> volumeLinks;
	Vector2f maxPoint, minPoint;
	std::vector<SceneSize> objectSizes;
	
};






