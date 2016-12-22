#include "Sprite.h"
#include <math.h>

std::map<std::string, sf::Texture*> Sprite::textures; // why pointer

Sprite::Sprite(std::string filename) {
	if (Sprite::textures.find(filename) == Sprite::textures.end()) {
		Sprite::textures[filename] = new sf::Texture;
		Sprite::textures[filename]->loadFromFile(filename);
	}		
	this->tex = textures[filename];
}

void Sprite::DrawScreenSpace(
	sf::RenderWindow *wnd, 
	Vector2f pos, 
	float ang, 
	Vector2f size,
	sf::Color color) 
{
	sf::Vertex vertices[4];
	Vector2f vertexPos;
	

	Vector2f xVector = Vector2f(cosf(ang), sinf(ang));
	Vector2f yVector = xVector.GetPerp();

	vertexPos = pos + xVector * size.x + yVector * size.y; // xi + yj
	vertices[0].position = sf::Vector2f(vertexPos.x, vertexPos.y);

	vertexPos = pos + xVector * size.x - yVector * size.y;
	vertices[1].position = sf::Vector2f(vertexPos.x, vertexPos.y);

	vertexPos = pos - xVector * size.x - yVector * size.y;
	vertices[2].position = sf::Vector2f(vertexPos.x, vertexPos.y);

	vertexPos = pos - xVector * size.x + yVector * size.y;
	vertices[3].position = sf::Vector2f(vertexPos.x, vertexPos.y);

	vertices[1].texCoords = sf::Vector2f(0.0f, 0.0f);
	vertices[2].texCoords = sf::Vector2f((float)tex->getSize().x, 0.0f);
	vertices[3].texCoords = sf::Vector2f((float)tex->getSize().x, (float)tex->getSize().y);
	vertices[0].texCoords = sf::Vector2f(0.0f, (float)tex->getSize().y);

	for (int i = 0; i < 4; i++) {
		vertices[i].color = color; //sf::Color::Color(255, 255, 255, 255);//sf::Color::White;
	}

	(*wnd).draw(vertices, 4, sf::Quads, tex);
}

/*void Sprite::LinkDrawScreenSpace(
	sf::RenderWindow *wnd,
	Vector2f pos0,
	Vector2f pos1,
	float width)
{
	sf::Vertex vertices[4];
	Vector2f vertexPos;


	Vector2f yVector = (pos1 - pos0).GetNorm();
	Vector2f xVector = yVector.GetPerp();

	vertexPos = pos0 + xVector * (width / 2.0f) - yVector * (width / 2.0f); // xi + yj
	vertices[0].position = sf::Vector2f(vertexPos.x, vertexPos.y);

	vertexPos = pos0 - xVector * (width / 2.0f) - yVector * (width / 2.0f);
	vertices[1].position = sf::Vector2f(vertexPos.x, vertexPos.y);

	vertexPos = pos1 - xVector * (width / 2.0f) + yVector * (width / 2.0f);
	vertices[2].position = sf::Vector2f(vertexPos.x, vertexPos.y);

	vertexPos = pos1 + xVector * (width / 2.0f) + yVector * (width / 2.0f);
	vertices[3].position = sf::Vector2f(vertexPos.x, vertexPos.y);

	vertices[1].texCoords = sf::Vector2f(0.0f, 0.0f);
	vertices[2].texCoords = sf::Vector2f((float)tex->getSize().x, 0.0f);
	vertices[3].texCoords = sf::Vector2f((float)tex->getSize().x, (float)tex->getSize().y);
	vertices[0].texCoords = sf::Vector2f(0.0f, (float)tex->getSize().y);

	for (int i = 0; i < 4; i++) {
		vertices[i].color = sf::Color::White;
	}

	(*wnd).draw(vertices, 4, sf::Quads, tex);
}*/

void Sprite::DrawWorldSpace(
	sf::RenderWindow *wnd,
	Vector2f pos,
	float ang,
	Vector2f size,
	Camera cam,
	sf::Color color)
{
	float pi = 3.14;
	Vector2f yVector = Vector2f(cam.ang);
	Vector2f xVector = Vector2f(cam.ang + pi / 2.0f);

	Vector2f delta = pos - cam.pos;
	Vector2f localPos = Vector2f(xVector * delta, yVector * delta); // координаты pos в системе координат камеры, xi+yj

	Vector2f screenPos;
	float fovx = cam.fieldOfView;
	float fovy = fovx * wnd->getSize().y / wnd->getSize().x;
	screenPos.x = (localPos.x / fovx + 0.5f) * float(wnd->getSize().x); //?
	screenPos.y = (localPos.y / fovy + 0.5f) * float(wnd->getSize().y);

	float screenAng = ang - cam.ang;
	Vector2f screenSize = size * (float(wnd->getSize().x) / fovx);

	this->DrawScreenSpace(wnd, screenPos, screenAng, screenSize, color);

}

/*void Sprite::LinkDrawWorldSpace(
	sf::RenderWindow *wnd,
	Vector2f pos0,
	Vector2f pos1,
	float width,
	Camera cam)
{
	float pi = 3.14;
	Vector2f yVector = Vector2f(cam.ang);
	Vector2f xVector = Vector2f(cam.ang + pi / 2.0f);

	Vector2f delta0 = pos0 - cam.pos;
	Vector2f delta1 = pos1 - cam.pos;
	Vector2f localPos0 = Vector2f(xVector * delta0, yVector * delta0);
	Vector2f localPos1 = Vector2f(xVector * delta1, yVector * delta1);

	Vector2f screenPos0;
	Vector2f screenPos1;
	float fovx = cam.fieldOfView;
	float fovy = fovx * wnd->getSize().y / wnd->getSize().x;

	screenPos0.x = (localPos0.x / fovx + 0.5f) * float(wnd->getSize().x); //?
	screenPos0.y = (localPos0.y / fovy + 0.5f) * float(wnd->getSize().y);

	screenPos1.x = (localPos1.x / fovx + 0.5f) * float(wnd->getSize().x); //?
	screenPos1.y = (localPos1.y / fovy + 0.5f) * float(wnd->getSize().y);

	this->LinkDrawScreenSpace(wnd, screenPos0, screenPos1, width);
}
*/



void Sprite::DrawBackground(
	sf::RenderWindow *wnd,
	float speed,
	Camera cam)
{
	cam.ang = -cam.ang; //!!!???

	sf::Vertex vertices[4];
	Vector2f vertexPos;

	vertices[0].position = sf::Vector2f(0.0f, 0.0f);
	vertices[1].position = sf::Vector2f(wnd->getSize().x, 0.0f);
	vertices[2].position = sf::Vector2f(wnd->getSize().x, wnd->getSize().y);
	vertices[3].position = sf::Vector2f(0.0f, wnd->getSize().y);
	for (int i = 0; i < 4; i++)
		vertices[i].color = sf::Color::White;
	tex->setRepeated(true);
	float pi = 3.14;
	Vector2f yVector = Vector2f(cam.ang);
	Vector2f xVector = Vector2f(cam.ang + pi / 2.0f);

	//vertices[0].texCoords = sf::Vector2f(cam.pos.x - cam.fieldOfView / 2.0f, )
	Vector2f tmpVector;
	
	tmpVector = speed*cam.pos - cam.fieldOfView / 2.0f * xVector - cam.fieldOfView / 2.0f * yVector * (wnd->getSize().y / wnd->getSize().x);
	vertices[0].texCoords = sf::Vector2f(tmpVector.x, tmpVector.y);

	//vertices[0].texCoords = sf::Vector2f(cam.pos.x - cam.fieldOfView / 2.0f, cam.pos.y - cam.fieldOfView / 2.0f * wnd->getSize().y / wnd->getSize().x);
	
	tmpVector = speed*cam.pos - cam.fieldOfView / 2.0f * xVector + cam.fieldOfView / 2.0f * yVector * (wnd->getSize().y / wnd->getSize().x);
	//vertices[1].texCoords = sf::Vector2f(cam.pos.x + cam.fieldOfView / 2.0f, cam.pos.y - cam.fieldOfView / 2.0f * wnd->getSize().y / wnd->getSize().x);
	vertices[1].texCoords = sf::Vector2f(tmpVector.x, tmpVector.y);

	tmpVector = speed*cam.pos + cam.fieldOfView / 2.0f * xVector + cam.fieldOfView / 2.0f * yVector * (wnd->getSize().y / wnd->getSize().x);
	//vertices[2].texCoords = sf::Vector2f(cam.pos.x + cam.fieldOfView / 2.0f, cam.pos.y + cam.fieldOfView / 2.0f * wnd->getSize().y / wnd->getSize().x);
	vertices[2].texCoords = sf::Vector2f(tmpVector.x, tmpVector.y);

	tmpVector = speed*cam.pos + cam.fieldOfView / 2.0f * xVector - cam.fieldOfView / 2.0f * yVector * (wnd->getSize().y / wnd->getSize().x);
	//vertices[3].texCoords = sf::Vector2f(cam.pos.x - cam.fieldOfView / 2.0f, cam.pos.y + cam.fieldOfView / 2.0f * wnd->getSize().y / wnd->getSize().x);
	vertices[3].texCoords = sf::Vector2f(tmpVector.x, tmpVector.y);

	wnd->draw(vertices, 4, sf::Quads, tex);
}