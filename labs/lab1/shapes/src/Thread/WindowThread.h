#ifndef WINDOWTHREAD_H
#define WINDOWTHREAD_H

#include <SFML/Graphics.hpp>
#include <memory>
#include <thread>

class WindowThread
{
public:
	static std::shared_ptr<sf::RenderWindow> CreateWindow()
	{
		auto window = std::make_shared<sf::RenderWindow>();
		std::thread windowThread(&WindowThread::RunWindow, window);
		windowThread.detach();
		return window;
	}

private:
	static void RunWindow(std::shared_ptr<sf::RenderWindow> window)
	{
		window->create(sf::VideoMode(800, 600), "Shapes");

		while (window->isOpen())
		{
			sf::Event event;
			while (window->pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window->close();
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}
};

#endif /* WINDOWTHREAD_H */
