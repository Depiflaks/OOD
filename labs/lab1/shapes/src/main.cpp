#include <SFML/Graphics.hpp>

#include <iostream>
#include <memory>

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

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
			gfx::MockCanvas canvas;
			Picture picture(canvas);
			StdCommandListener listener;
			ShapesCommandLine app;

			app.Start(picture, listener);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Fatal error: " << e.what() << std::endl;
			return 1;
		}

		window.clear();
		window.draw(shape);
		window.display();
	}

	return 0;
}
