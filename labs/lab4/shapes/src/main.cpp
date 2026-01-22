#include <SFML/Graphics.hpp>
#include <iostream>

#include "lib/Designer.h"
#include "lib/Painter.h"
#include <fstream>
#include <memory>

int main()
{
	try
	{
		std::ifstream inputFile("input.txt");
		if (!inputFile.is_open())
		{
			throw std::runtime_error("Cannot open input.txt");
		}

		auto factory = std::make_unique<ShapeFactory>();
		Designer designer(std::move(factory));
		PictureDraft draft = designer.CreateDraft(inputFile);
		inputFile.close();

		sf::RenderWindow window(sf::VideoMode(800, 600), "Shapes Drawing");
		SFMLCanvas canvas(window);

		window.clear(sf::Color::White);

		Painter painter;
		painter.DrawPicture(draft, canvas);

		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

// из документа удалённые элементы не должны отображаться
// в списке съехали индексы
