#include <SFML/Graphics.hpp>

#include "./Command/CommandListener.h"
#include "./GFX/SFMLCanvas.h"
#include "./Shapes/ShapesCommandLine.h"
#include <iostream>

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "Shapes");

	gfx::SFMLCanvas canvas{ window };
	shapes::Picture picture(canvas);
	commands::StdCommandListener listener;
	shapes::ShapesCommandLine app;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		try
		{
			app.ExecuteCommand(picture, listener);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Fatal error: " << e.what() << std::endl;
			return 1;
		}

		window.clear();
		window.display();
	}

	return 0;
}
