#ifndef CONSOLETHREAD_H
#define CONSOLETHREAD_H

#include "../Command/CommandListener.h"
#include "../GFX/SFMLCanvas.h"
#include "../Shapes/ShapesCommandLine.h"
#include <SFML/Graphics.hpp>
#include <atomic>
#include <iostream>
#include <thread>

class ConsoleThread
{
public:
	ConsoleThread(sf::RenderWindow& window, commands::ICommandListener& listener)
		: m_window(window)
		, m_canvas(window)
		, m_picture(m_canvas)
		, m_running(false)
		, m_listener(listener)
	{
	}

	void Start()
	{
		m_running = true;
		m_thread = std::thread(&ConsoleThread::Run, this);
	}

	void Join()
	{
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	void Stop()
	{
		m_running = false;
	}

private:
	void Run()
	{
        std::cout << (m_window.isOpen() ? "123\n" : "321\n");
		while (m_running && m_window.isOpen())
		{
			try
			{
				m_app.ExecuteCommand(m_picture, m_listener);
			}
			catch (const std::exception& e)
			{
				std::cerr << "Error: " << e.what() << std::endl;
			}
		}
	}

	sf::RenderWindow& m_window;
	gfx::SFMLCanvas m_canvas;
	commands::ICommandListener& m_listener;
	shapes::Picture m_picture;
	shapes::ShapesCommandLine m_app;
	std::thread m_thread;
	std::atomic<bool> m_running;
};

#endif /* CONSOLETHREAD_H */
