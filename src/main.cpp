#include "SFML/Graphics.hpp"
#include <iostream>
#include "GameSystem.h"


int main()
{
	std::cout << "Hello, World!\n";
	sf::RenderWindow window(sf::VideoMode(800, 800),"Experiments!");
	GameSystem sys(&window);
	while (window.isOpen())
	{
		sys.Update(1E-2f);
		sys.Draw();
		window.display();
		window.clear();
	}
	return 0;
}