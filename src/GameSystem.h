#pragma once
#include <vector>
#include "ParticleSystem.h"
#include "SFML/Graphics.hpp"
#include "Sprite.h"
#include "json/json.h"


class Plank;
class TmpPlank;
class InitialParticle;
//class Ship;
//class Grid;

class GameObject {
public:
	virtual ~GameObject() {}
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
	virtual bool Exists() = 0;
	bool exists;
};

class GameSystem {
public:
	GameSystem::GameSystem(sf::RenderWindow *wnd);
	void Update(float dt);
	void Draw();
	void SolvePlanks();
	void AddObject(GameObject* newObject);
	void Save();
	void Load();
	void DeleteLevel();
	void LoadLevel(std::string levelFileName);
	GameObject* GetObjecta(int index);
	ParticleSystem *GetParticleSystem();
	sf::RenderWindow *GetWindow();
	Camera cam;
	bool testMode = false;
	bool linkAddingActive = false;
	//Grid * grid;
	Vector2f linkPrevParticlePos = Vector2f(0.0f, 0.0f);
	float clickTime;
	float minClickTime;
	void DeletePlank();
	void NewGame();
	void MoveCamera();
	int GetPlankIndexJSON(Json::Value root, Plank* plank);
private:
	std::vector<GameObject*> objects;
	std::vector<Plank* > planks; // Дублирует из objects
	ParticleSystem *sys;
	sf::RenderWindow *wnd;
	float gridStep;
	float maxLength;
	bool prevStepCarExists;
	bool carExists;
	float timeLinkAdding;
	TmpPlank *tmpPlank;
	bool addingNonPhysicalPlanks = false;
	float totalPlankLengthAvailible;
	float defTotalPlankLengthAvailible;
	//Background* newBackground;
	float xCameraLimit; // нужно переписать
	float yCameraLimit;

	Json::Value saveRoot;
};