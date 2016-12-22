#pragma once
#include "SFML/Graphics.hpp"
#include "Vector2f.h"
#include <string>
#include <map>

class Camera {
public:
	Vector2f pos;
	float ang;
	float fieldOfView; // ширина, которую камера видит по горизонтали
};

class Sprite 
{
public:
	Sprite() {};
	Sprite(std::string filename);
	void DrawScreenSpace(
		sf::RenderWindow *wnd, 
		Vector2f screenPos, 
		float screenAng, 
		Vector2f screenSize,
		sf::Color color);
	/*void LinkDrawScreenSpace(
		sf::RenderWindow *wnd,
		Vector2f pos0,
		Vector2f pos1,
		float width);*/
	void DrawWorldSpace(
		sf::RenderWindow *wnd,
		Vector2f pos,
		float ang,
		Vector2f size,
		Camera cam,
		sf::Color color);
	/*void LinkDrawWorldSpace( // !!! можно было сделать через обычный DrawWorldSpace, используя size
		sf::RenderWindow *wnd,
		Vector2f pos0,
		Vector2f pos1,
		float width,
		Camera cam*/
	void Sprite::DrawBackground(
		sf::RenderWindow *wnd,
		float speed,
		Camera cam);
		sf::Texture *tex; //?
private:
	static std::map<std::string, sf::Texture*>  textures; //static - общее поле для всех объектов класса
};