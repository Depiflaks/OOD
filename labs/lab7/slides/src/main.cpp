//
// Created by smmm on 21.11.2025.
//

#include "lib/Canvas.h"
#include "lib/Shapes/GroupShape.h"
#include "lib/Shapes/Shapes.h"
#include <memory>

Slide CreateScene()
{
	auto shapes = std::make_shared<GroupShape>();

	auto houseBase = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeRectangleStrategy()),
		RectD{ 100, 100, 200, 150 }, std::make_unique<Style>(true, 0xFFFF0000),
		std::make_unique<Style>(true, 0xFF000000), 2);

	auto roof = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakePolygonStrategy(3)),
		RectD{ 150, 50, 100, 80 }, std::make_unique<Style>(true, 0xFF8B4513),
		std::make_unique<Style>(true, 0xFF000000), 2);

	auto window = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeRectangleStrategy()),
		RectD{ 150, 130, 50, 40 }, std::make_unique<Style>(true, 0xFF87CEEB),
		std::make_unique<Style>(true, 0xFF000000), 1);

	auto door = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeRectangleStrategy()),
		RectD{ 220, 180, 40, 70 }, std::make_unique<Style>(true, 0xFF8B4513),
		std::make_unique<Style>(true, 0xFF000000), 1);

	auto houseGroup = std::make_shared<GroupShape>();
	houseGroup->InsertShape(houseBase);
	houseGroup->InsertShape(roof);
	houseGroup->InsertShape(window);
	houseGroup->InsertShape(door);

	auto treeTrunk = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeRectangleStrategy()),
		RectD{ 350, 150, 20, 100 }, std::make_unique<Style>(true, 0xFF8B4513),
		std::make_unique<Style>(true, 0xFF000000), 1);

	auto treeCrown = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeEllipseStrategy()),
		RectD{ 320, 80, 80, 80 }, std::make_unique<Style>(true, 0xFF228B22),
		std::make_unique<Style>(true, 0xFF000000), 1);

	auto sun = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeEllipseStrategy()),
		RectD{ 50, 50, 60, 60 }, std::make_unique<Style>(true, 0xFFFFFF00),
		std::make_unique<Style>(true, 0xFFFFA500), 3);

	shapes->InsertShape(houseGroup);
	shapes->InsertShape(treeTrunk);
	shapes->InsertShape(treeCrown);
	shapes->InsertShape(sun);

	return Slide{ 800, 600, shapes };
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Canvas Demo");
	SFMLCanvas canvas(window);

	auto slide = CreateScene();

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color(240, 240, 240));

		slide.Draw(canvas);

		window.display();
	}

	return 0;
}
