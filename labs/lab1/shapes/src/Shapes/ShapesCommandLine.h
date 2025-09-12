#ifndef SHAPESCOMMANDLINE_H
#define SHAPESCOMMANDLINE_H

class ShapesCommandLine
{
public:
	void Start(Picture& picture, ICommandListener& listener)
	{
		RunWithErrorHandling(picture, listener);
	}

private:
	void RunWithErrorHandling(Picture& picture, ICommandListener& listener)
	{
		while (true)
		{
			try
			{
				ExecuteCommand(picture, listener);
			}
			catch (const std::runtime_error& e)
			{
				std::cout << "Error: " << e.what() << std::endl;
			}
			catch (...)
			{
				std::cout << "Unknown error occurred" << std::endl;
			}
		}
	}

	void ExecuteCommand(Picture& picture, ICommandListener& listener)
	{
		std::string commandLine = listener.GetLine();
		if (commandLine.empty())
		{
			return;
		}

		CommandExecutor executor;
		executor.Execute(picture, commandLine);
	}
};

#endif /* SHAPESCOMMANDLINE_H */
