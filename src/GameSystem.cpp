#include "GameSystem.h"
#include <iostream>
#include <typeinfo>
#include <string.h>
#include "Background.h"
#include "Grid.h"
#include "Plank.h"
#include "Car.h"
#include "TmpPlank.h"
#include "InitialParticle.h"

#include "json/writer.h"
#include <fstream>

class Plank;
void SolveTwoPlanks(Plank *plank0, Plank *plank1);

GameSystem::GameSystem(sf::RenderWindow *wnd)
{
	cam.pos.x = (wnd->getSize().x) / 2.0f;
	cam.pos.y = (wnd->getSize().y) / 2.0f;
	cam.fieldOfView = wnd->getSize().x;
	cam.ang = 0;
	gridStep = 40.0f;
	maxLength = 1201.0f;
	carExists = false;
	//prevStepCarExists = false;
	linkAddingActive = false;
	timeLinkAdding = 0.0f;
	clickTime = 0.0f;
	minClickTime = 0.5f;
	defTotalPlankLengthAvailible = 200000.0f;
	totalPlankLengthAvailible = defTotalPlankLengthAvailible;
	std::cout << "totalPlankLengthAvailible (constructor): " << totalPlankLengthAvailible  << "\n";



	this->wnd = wnd;

	sys = new ParticleSystem(Vector2f(0.0f, 0.0f), Vector2f(800.0f, 800.0f));
	
	LoadLevel("data/level.json");
}

void GameSystem::AddObject(GameObject* newObject)
{
	objects.push_back(newObject);
	std::cout << "Object added\n";
	//if (typeid(*newObject).name() == "class Plank")
	if (!strcmp(typeid(*newObject).name(), "class Plank"))
	{
		planks.push_back((Plank *)newObject);
		std::cout << "Plank added\n";
	}
	//std::cout << typeid(typeid(*newObject).name()).name() << '\n';
	//std::cout << typeid(*newObject).name() << '\n';
}

void GameSystem::Update(float dt)
{
	MoveCamera();
	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
	{
		cam.pos.y++;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
	{
		cam.pos.y--;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
	{
		cam.pos.x--;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
	{
		cam.pos.x++;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		//cam.fieldOfView--;
		cam.fieldOfView = cam.fieldOfView * 0.995;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
	{
		//cam.fieldOfView++;
		cam.fieldOfView = cam.fieldOfView * 1.005;
	}*/

	/*if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
	{
		testMode = true;
		//std::cout << "isTestActive\n";
	}*/

	// Setting mouse pointer
	Vector2f posScreenCoord = Vector2f(cam.fieldOfView * sf::Mouse::getPosition(*wnd).x / wnd->getSize().x,
		cam.fieldOfView * sf::Mouse::getPosition(*wnd).y / wnd->getSize().x);
	Vector2f xVector = Vector2f(cam.ang);
	Vector2f yVector = Vector2f(cam.ang + 3.1415926 / 2.0f);
	Vector2f posScreenZero = cam.pos - (xVector * cam.fieldOfView) * 0.5f -
		(yVector * cam.fieldOfView * (GetWindow()->getSize().y / GetWindow()->getSize().x)) * 0.5f;
	Vector2f position = posScreenZero + posScreenCoord;

	float step = gridStep;
	if (fabs(position.x - floor(position.x / step) * step) < step / 2)
		position.x = floor(position.x / step) * step;
	else
		position.x = floor(position.x / step) * step + step;
	if (fabs(position.y - floor(position.y / step) * step) < step / 2)
		position.y = floor(position.y / step) * step;
	else
		position.y = floor(position.y / step) * step + step;
	//Working with planks
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	{
		// Starting adding a new plank
		if ((!linkAddingActive) && (GetParticleSystem()->ParticleIndex(position) != -1) && (timeLinkAdding > 0.5f))
		{
			linkPrevParticlePos = position;
			tmpPlank->pos0 = linkPrevParticlePos;
			tmpPlank->pos1 = linkPrevParticlePos;
			linkAddingActive = true;
			std::cout << "Started creating a plank\n";
			std::cout << position.x << " " << position.y << "\n";
		}
		// Finishing adding a new plank
		if (((position - linkPrevParticlePos).Length() > 0.001f) && (linkPrevParticlePos.Length() > 0) &&
			((position - linkPrevParticlePos).Length() < maxLength) && (position - linkPrevParticlePos).Length() < totalPlankLengthAvailible)
		{
			Plank* newPlank = new Plank(this, linkPrevParticlePos, position);
			AddObject(newPlank);
			if (addingNonPhysicalPlanks)
				newPlank->link->isPhysical = false;

			linkAddingActive = false;
			timeLinkAdding = 0.0f;

			totalPlankLengthAvailible -= (position - linkPrevParticlePos).Length();
			std::cout << "totalPlankLengthAvailible: " << totalPlankLengthAvailible << " " << (position - linkPrevParticlePos).Length() << "\n";


			int size = objects.size();
			Json::Value pos0(Json::arrayValue);
			pos0.append(Json::Value(linkPrevParticlePos.x));
			pos0.append(Json::Value(linkPrevParticlePos.y));
			Json::Value pos1(Json::arrayValue);
			pos1.append(Json::Value(position.x));
			pos1.append(Json::Value(position.y));
			saveRoot["objects"][size]["type"] = "plank";
			saveRoot["objects"][size]["pos0"] = pos0;
			saveRoot["objects"][size]["pos1"] = pos1;
			if (addingNonPhysicalPlanks)
				saveRoot["objects"][size]["isPhysical"] = false;
			else
				saveRoot["objects"][size]["isPhysical"] = true;

			//std::cout << "\n\n";
			//std::cout << saveRoot << std::endl;
			//std::cout << "\n\n";

			linkPrevParticlePos = Vector2f(0.0f, 0.0f);
			std::cout << "Finished creating a plank\n";
			std::cout << position.x << " " << position.y << "\n";
		}
	}
	if (linkAddingActive)
	{
		if ((position - tmpPlank->pos0).Length() < maxLength && 
			(position - linkPrevParticlePos).Length() < totalPlankLengthAvailible)
			tmpPlank->pos1 = position;
	}
	

	sf::Event evt;
	while (wnd->pollEvent(evt)) {
		if (evt.type == sf::Event::Closed)
			wnd->close();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
			wnd->close();
		if (evt.type == sf::Event::KeyPressed)
		{
			if (evt.key.code == sf::Keyboard::C)
			{
				Car * newCar = new Car(this, position, Vector2f(100.0f, 50.0f));
				AddObject(newCar);
			}
			if (evt.key.code == sf::Keyboard::S)
			{
				Save();
				testMode = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F1))
			{
				Save();
				std::cout << "Game saved\n";
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F9))
			{
				DeleteLevel();
				std::cout << "Objects deleted\n";
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F10))
			{
				DeleteLevel();
				LoadLevel("data/level.json");
				Save();
				std::cout << "Objects of leve 1 loaded\n";
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F11))
			{
				DeleteLevel();
				LoadLevel("data/Level2.json");
				Save();
				std::cout << "Objects of level 2 loaded\n";
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::F5))
			{
				NewGame();
				Load();
				std::cout << "New game\n";
			}
			if (evt.key.code == sf::Keyboard::P)
			{
				if (!addingNonPhysicalPlanks)
				{
					addingNonPhysicalPlanks = true;
					std::cout << "Adding non-physical planks\n";
				}
				else
				{
					addingNonPhysicalPlanks = false;
					std::cout << "Adding physical planks\n";
				}
			}
			if (evt.key.code == sf::Keyboard::R)
			{
				NewGame();
			}
		}
		if (evt.type == sf::Event::MouseButtonPressed)
		{
			if (evt.key.code == sf::Mouse::Right)
			{
				DeletePlank();
				std::cout << "Right mouse button was pressed\n";
			}
		}
	};

	/*if (sf::Mouse::isButtonPressed(sf::Mouse::Right) && (carExists == false))
	{
		carExists = true;
		//Car * newCar = new Car(this, position, Vector2f(100.0f, 50.0f));
		Car * newCar = new Car(this, position, Vector2f(100.0f, 50.0f));
		AddObject(newCar);
	}*/
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z) && (clickTime > minClickTime))
	{
		if (planks.size() > 1) planks[0]->exists = false;
		//tmpPlank->exists = false;
		clickTime = 0.0f;
	}

	for (unsigned int i = 0; i < planks.size();)
	{
		if (!planks[i]->Exists())
		{
			planks[i] = planks.back();
			planks.pop_back();
		}
		else
		{
			i++;
		}
	}
	int a = objects.size();
	for (unsigned int i = 0; i < objects.size();)
	{
		if (!objects[i]->Exists())
		{
			objects[i]->Update(dt);
			delete objects[i]; // освобождаетс€ пам€ть, вызываетс€ деструктор
			objects[i] = objects.back();
			objects.pop_back();
			
		}
		else
		{
			objects[i]->Update(dt);
			i++;
		}
	}
//	for (unsigned int i = 0; i < objects.size(); i++)
	//{
		//objects[i]->Update(dt);
	//}
	if (this->testMode)
		sys->SolvePhysicalLinks(dt);
	timeLinkAdding += dt;
	clickTime += dt;
}

void GameSystem::Draw() {
	for (unsigned int i = 0; i < objects.size(); i++) {
		objects[i]->Draw();
	}
}

ParticleSystem *GameSystem::GetParticleSystem() {
	return this->sys;
}

sf::RenderWindow *GameSystem::GetWindow() {
	return wnd;
}

GameObject* GameSystem::GetObjecta(int index) {
	return objects[index];
}

void GameSystem::SolvePlanks()
{
	for (int i = 0; i < planks.size(); i++)
		for (int j = i + 1; j < planks.size(); j++)
		{
			SolveTwoPlanks(planks[i], planks[j]);
		}
}

void GameSystem::DeletePlank()
{
	float pi = 3.14;
	Vector2f xVector = Vector2f(cam.ang);
	Vector2f yVector = Vector2f(cam.ang + pi / 2.0f);
	Vector2f localPos = Vector2f(sf::Mouse::getPosition(*wnd).x, sf::Mouse::getPosition(*wnd).y);
	float fovx = cam.fieldOfView;
	float fovy = fovx * wnd->getSize().y / wnd->getSize().x;
	Vector2f globalPos = cam.pos + (localPos.x - fovx / 2) * xVector + (localPos.y - fovy / 2) * yVector;
	//Vector2f globalPos = localPos;
	globalPos.Print();
	Plank* plank;
	for (int plankIndex = 0; plankIndex < planks.size(); plankIndex++)
	{
		plank = planks[plankIndex];
		if ((globalPos.x - plank->link->p0->pos.x) * (globalPos.x - plank->link->p1->pos.x) > plank->GetWidth() * plank->GetWidth())
			continue;
		if ((globalPos.y - plank->link->p0->pos.y) * (globalPos.y - plank->link->p1->pos.y) > plank->GetWidth() * plank->GetWidth())
			continue;
		float a = (globalPos - plank->link->p0->pos).Length();
		float b = (globalPos - plank->link->p1->pos).Length();
		float c = (plank->link->p0->pos - plank->link->p1->pos).Length();
		float p = (a + b + c) / 2.0f;
		float square = sqrt(p * (p - a) * (p - b) * (p - c));
		float h = square / c;
		if (h < plank->GetWidth())
		{
			plank->exists = false;
			totalPlankLengthAvailible += (plank->link->p0->pos - plank->link->p1->pos).Length();

			std::fstream levelTestFile("data/levelTest.json");
			Json::Reader jsonReader;
			if (jsonReader.parse(levelTestFile, saveRoot))
				std::cout << "DeletePlank parsing success\n";
			else
				std::cout << "DeletePlank parsing error\n" << jsonReader.getFormattedErrorMessages();
			int index = GetPlankIndexJSON(saveRoot, plank);
			Json::Value tmp;
			saveRoot["objects"].removeIndex(index, &tmp);
			std::cout << index << "\n";

			//removeIndex(ArrayIndex i, Value* removed);
			//std::cout << "Plank deleted\n";
		}
	}

}

void GameSystem::NewGame()
{
	testMode = false;
	for (int plankIndex = 0; plankIndex < planks.size(); plankIndex++)
	{
		planks[plankIndex]->exists = false;
	}
	totalPlankLengthAvailible = defTotalPlankLengthAvailible;
}

void GameSystem::Save()
{
	//std::fstream levelTestFile("data/levelTest.json");
	std::fstream levelTestFile;
	levelTestFile.open("data/levelTest.json", std::ofstream::out | std::ofstream::trunc);
	levelTestFile << saveRoot << std::endl;
	levelTestFile.close();
	
}

void GameSystem::Load()
{
	std::fstream levelTestFile("data/levelTest.json");

	Json::Reader jsonReader;

	if (jsonReader.parse(levelTestFile, saveRoot))
		std::cout << "Load parsing success\n";
	else
		std::cout << "Load parsing error\n" << jsonReader.getFormattedErrorMessages();

	Json::Value objects = saveRoot["objects"];

	int objectsCount = objects.size();
	Vector2f particleSize = Vector2f(10.0f, 3.0f);
	for (int i = 0; i < objectsCount; i++)
	{
		if (objects[i]["type"] == "plank")
		{
			Vector2f pos0 = Vector2f(objects[i]["pos0"][0].asFloat(), objects[i]["pos0"][1].asFloat());
			Vector2f pos1 = Vector2f(objects[i]["pos1"][0].asFloat(), objects[i]["pos1"][1].asFloat());
			Particle * particle0;
			Particle * particle1;
			int a = GetParticleSystem()->ParticleIndex(pos1);
			if(GetParticleSystem()->ParticleIndex(pos0) == -1)
				particle0 = GetParticleSystem()->AddParticle(pos0, particleSize.Length()*0.5f);
			else
				particle0 = GetParticleSystem()->GetParticle(GetParticleSystem()->ParticleIndex(pos0));
			if (GetParticleSystem()->ParticleIndex(pos1) == -1)
				particle1 = GetParticleSystem()->AddParticle(pos1, particleSize.Length()*0.5f);
			else
				particle1 = GetParticleSystem()->GetParticle(GetParticleSystem()->ParticleIndex(pos1));
			Plank* newPlank = new Plank(this, particle0->pos, particle1->pos);
			AddObject(newPlank);
			if (objects[i]["isPhysical"] == false)
				newPlank->link->isPhysical = false;
		}
	}
}

void GameSystem::DeleteLevel() 
{
	for (int i = 0; i < objects.size(); i++)
		delete objects[i];
	objects.clear();
	planks.clear();
}
void GameSystem::LoadLevel(std::string levelFileName)
{
	std::fstream levelFile(levelFileName);

	Json::Reader jsonReader;

	Json::Value root;

	if (jsonReader.parse(levelFile, root))
		std::cout << "Success\n";
	else
		std::cout << "Parsing error\n" << jsonReader.getFormattedErrorMessages();

	std::string backgroundTextureName = root["backgroundTexture"]["name"].asString();
	Background * newBackground = new Background(this, backgroundTextureName);
	AddObject(newBackground);
	xCameraLimit = newBackground->sprite.tex->getSize().x;
	yCameraLimit = newBackground->sprite.tex->getSize().y;

	Json::Value objects = root["objects"];
	int objectsCount = objects.size();
	Vector2f particleSize = Vector2f(10.0f, 3.0f);
	Vector2f initialParticlePos = Vector2f(0.0f, 0.0f);
	InitialParticle * initialParticle;

	for (int i = 0; i < objectsCount; i++)
	{
		if (objects[i]["type"] == "link")
		{
			Vector2f pos0 = Vector2f(objects[i]["pos0"][0].asFloat(), objects[i]["pos0"][1].asFloat());
			Vector2f pos1 = Vector2f(objects[i]["pos1"][0].asFloat(), objects[i]["pos1"][1].asFloat());
			Particle * particle0 = GetParticleSystem()->AddParticle(pos0, particleSize.Length()*0.5f);
			Particle * particle1 = GetParticleSystem()->AddParticle(pos1, particleSize.Length()*0.5f);
			GetParticleSystem()->FixParticle(GetParticleSystem()->ParticleIndex(pos0));
			GetParticleSystem()->FixParticle(GetParticleSystem()->ParticleIndex(pos1));
			GetParticleSystem()->AddLink(particle0, particle1, 0.05f);
		}
		else if (objects[i]["type"] == "initialParticle")
		{
			initialParticlePos = Vector2f(objects[i]["pos0"][0].asFloat(), objects[i]["pos0"][1].asFloat());
			initialParticle = new InitialParticle(this, initialParticlePos);
			AddObject(initialParticle);
		}
	}
	Grid * newGrid = new Grid(this, "data/grid2.png", "data/circle.png", gridStep, particleSize);
	AddObject(newGrid);

	//initialParticle1 = new InitialParticle(this, visibleParticle1);
	//AddObject(initialParticle1);
	//initialParticle0 = new InitialParticle(this, visibleParticle0);
	//AddObject(initialParticle0);

	std::cout << "Objects added";
	tmpPlank = new TmpPlank(this, Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f));
	AddObject(tmpPlank);

	saveRoot.clear();
	Save();
}

int GameSystem::GetPlankIndexJSON(Json::Value root, Plank* plank)
{
	for (int i = 0; i < root["objects"].size(); i++)
	{
		if ((root["objects"][i]["type"] == "plank")
			&& (root["objects"][i]["pos0"][0].asFloat() == plank->link->p0->pos.x)
			&& (root["objects"][i]["pos0"][1].asFloat() == plank->link->p0->pos.y)
			&& (root["objects"][i]["pos1"][0].asFloat() == plank->link->p1->pos.x)
			&& (root["objects"][i]["pos1"][1].asFloat() == plank->link->p1->pos.y))
			return i;
	}
	return -1;
}

void GameSystem::MoveCamera()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && (cam.pos.y + cam.fieldOfView / 2 < yCameraLimit))
	{
		cam.pos.y++;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && (cam.pos.y - cam.fieldOfView / 2 > 0.0f))
	{
		cam.pos.y--;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && (cam.pos.x - cam.fieldOfView / 2 > 0.0f))
	{
		cam.pos.x--;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && (cam.pos.x + cam.fieldOfView / 2 < xCameraLimit))
	{
		cam.pos.x++;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
	{
		cam.fieldOfView = cam.fieldOfView * 0.995;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
		{
			cam.pos.x = xCameraLimit / 2.0f;
			cam.pos.y = yCameraLimit / 2.0f;
			cam.fieldOfView = yCameraLimit;
		}
	}
	//float xLimit = Background->

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::N)
		&& (cam.pos.y + cam.fieldOfView / 2 < yCameraLimit)
		&& (cam.pos.y - cam.fieldOfView / 2 > 0.0f)
		&& (cam.pos.x - cam.fieldOfView / 2 > 0.0f)
		&& (cam.pos.x + cam.fieldOfView / 2 < xCameraLimit))
	{
		//cam.fieldOfView++;
		cam.fieldOfView = cam.fieldOfView * 1.005;
	}
}