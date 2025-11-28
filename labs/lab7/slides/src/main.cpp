//
// Created by smmm on 21.11.2025.
//

#include "lib/Canvas.h"
#include "lib/Shapes/GroupShape.h"
#include "lib/Shapes/Shapes.h"


void CreateScene(std::vector<std::shared_ptr<IDrawable>>& drawables)
{
	auto circle = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeEllipseStrategy()),
		RectD{ 50, 50, 100, 100 }, std::make_unique<Style>(true, 0xFF0000FF),
		std::make_unique<Style>(true, 0x000000FF), 5);
	drawables.push_back(circle);

	auto rect = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeRectangleStrategy()),
		RectD{ 200, 50, 150, 80 }, std::make_unique<Style>(true, 0x00FF00FF),
		std::make_unique<Style>(true, 0x808080FF), 2);
	drawables.push_back(rect);

	auto group = std::make_shared<GroupShape>();
	group->InsertShape(circle->Clone());

	auto smallRect = std::make_shared<SimpleShape>(
		std::make_shared<DrawingStrategy>(MakeRectangleStrategy()),
		RectD{ 0, 0, 30, 30 }, std::make_unique<Style>(true, 0x0000FFFF),
		std::make_unique<Style>(false, 0x000000FF), 1);
	group->InsertShape(smallRect);

	group->SetFrame(RectD{ 300, 200, 200, 150 });
	group->SetOutlineThickness(3);
	group->SetOutlineColor(0xFF00FFFF);

	drawables.push_back(group);
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Canvas Demo");
	SFMLCanvas canvas(window);

	std::vector<std::shared_ptr<IDrawable>> drawables;
	CreateScene(drawables);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(sf::Color(240, 240, 240));

		for (const auto& drawable : drawables)
		{
			drawable->Draw(canvas);
		}

		window.display();
	}

	return 0;
}
